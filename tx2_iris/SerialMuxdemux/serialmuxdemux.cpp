#include <memory>
#include <string>
#include <string.h>
#include <unistd.h>
#include "serialmuxdemux.h"
//#include "../serial/include/basictypes.h"
//#include "../serial/include/asyncserial.h"

//#define DEBUG
#include <stdio.h>

using namespace std;

SerialMuxDemux* SerialMuxDemux::s_ins = NULL;
std::mutex SerialMuxDemux::s_mutex;
std::map<unsigned char, std::map<int, Callback>> SerialMuxDemux::s_callbacks;
std::mutex SerialMuxDemux::s_refMutex;
int SerialMuxDemux::s_refCount = 0;
//static AsyncSerial* SerialMuxDemux::_asyncSerial = NULL;


SerialMuxDemux::SerialMuxDemux():
    _hasInited(false),
    _asyncSerial(NULL),
    _rdPos(0),
    _wrPos(0),
    _isEmpty(true),
    _isFull(false),
    _enableSend(false)
{
    //TO DO
    s_refCount = 0;
    s_callbacks.clear();

    memset(_dataBuf, 0, SEND_BUFFER_SIZE);
    _asyncSerial = new AsyncSerial();
}

SerialMuxDemux::~SerialMuxDemux()
{
    //TO DO
    if(NULL != _asyncSerial)
    {
        std::cout << "[SerialMuxDemux]: closing serial..." << std::endl;
        if(true == _enableSend)
        {
            _enableSend = false;       //终止发送
            pthread_join(_thrID, NULL);
        }

        int serialClosed = s_ins->_asyncSerial->StopRecv();
        std::cout << "[SerialMuxDemux]:" << (E_SERIAL_OK == serialClosed ? "serial is closed successfully!" : "failed to close serial!!!")
                  << std::endl;

        delete _asyncSerial;
    }
}

SerialMuxDemux* SerialMuxDemux::GetInstance()
{
    if(NULL == s_ins)
    {
        std::lock_guard<mutex> lockGuard(s_mutex);
        if(NULL == s_ins)
        {
            s_ins = new SerialMuxDemux();
        }
    }

    //更新实例引用计数
    std::lock_guard<mutex> lockGurad(s_refMutex);
    s_refCount++;

    return s_ins;
}

void SerialMuxDemux::ReleaseInstance()
{
    //TO DO
    std::lock_guard<mutex> lockGurad(s_refMutex);
    if(0 == --s_refCount)
    {
        delete s_ins;
        s_ins = NULL;
    }
}

int SerialMuxDemux::Init(const char* port, SerialConfig* cfg)
{
    //TO DO
    //仅初始化一次
    if(_hasInited)
    {
        return E_SERIAL_OK;
    }


    //判断串口是否有效
    if(nullptr == _asyncSerial)
    {
    	return E_SERIAL_INVALID_PORT;
    }


    std::string sPort(port);
    //打开和配置串口
    if(NULL == port)
    {
        //return E_SERIAL_INVALID_PORT;
    	sPort = "ttys1";
    }



    //打开串口，并注册串口数据分发回调函数
    int rtnVal =  _asyncSerial->OpenSerial(sPort.c_str(), CallbackDemux);
    if(rtnVal != E_SERIAL_OK)
    {
        return rtnVal;
    }

    //配置串口
    SerialConfig serialCfg;
    if(nullptr != cfg)
    {
        serialCfg = *cfg;
    }

    rtnVal = _asyncSerial->SetConfig(serialCfg);
//    printf("Serial Config: %d %d %d %d\n",serialCfg.SerialBaudRate, serialCfg.SerialParityMode, serialCfg.SerialDataBits, serialCfg.SerialStopBits);

    if(rtnVal != E_SERIAL_OK)
    {
        return rtnVal;
    }

    rtnVal = _asyncSerial->StartRecv();
    if(rtnVal != E_SERIAL_OK)
    {
        return rtnVal;
    }


    //启动发送线程
    _enableSend = true;
    rtnVal = pthread_create(&_thrID, NULL, SendToSerial, this);
    if(E_SERIAL_OK != rtnVal)
    {
        return rtnVal;
    }

    _hasInited = true;
    return  rtnVal;
}


int SerialMuxDemux::Send(const unsigned char *data, const int size)
{
    //TO DO
    if(_hasInited)
    {
#ifdef DEBUG
        printf("sending data: 0x");
        for(int i = 0; i < size; i++)
        {
            printf("%02X", data[i]);
        }
        printf("\n");
#endif

        std::lock_guard<mutex> lockGuard(s_mutex);
        return WriteToBuffer(data, size);
    }

    return E_SERIAL_INVALID_PTR;
}

bool SerialMuxDemux::AddCallback(unsigned char cmd, Callback callback)
{
    //TO DO
#ifdef DEBUG
    printf("add callback cmd: 0x%x\n", cmd);
#endif

    int callbackID;
    return AddCallback(cmd, callback, callbackID);
//    if(ExistCallback(s_callbacks[cmd], callback, callbackID))
//    {
//    	printf("[%s@SerialMuxDemux]: this callback has alread enrolled\n", __FUNCTION__);
//    	return true;
//    }
//
//    callbackID = GetUsableKey(s_callbacks[cmd]);
//    std::lock_guard<mutex> lockGuard(s_mutex);
//    s_callbacks[cmd][callbackID] = callback;
//
//    printf("[%s@SerialMuxDemux]: add a new callback with: cmd = 0x%02X  callbackid = %d\n", __FUNCTION__, cmd, callbackID);

//    return s_callbacks.count(cmd) > 0 && s_callbacks[cmd].count(callbackID) > 0;
}

bool SerialMuxDemux::AddCallback(IN unsigned char cmd, IN Callback callback, OUT int &callbackID)
{
#ifdef DEBUG
    printf("add callback cmd: 0x%x\n", cmd);
#endif

    if(ExistCallback(s_callbacks[cmd], callback, callbackID))
    {
    	printf("[%s@SerialMuxDemux]: this callback has alread enrolled\n", __FUNCTION__);
    	return true;
    }

    callbackID = GetUsableKey(s_callbacks[cmd]);
    std::lock_guard<mutex> lockGuard(s_mutex);
    s_callbacks[cmd][callbackID] = callback;
    printf("[%s@SerialMuxDemux]: add a new callback with: cmd = 0x%02X  callbackid = %d\n", __FUNCTION__, cmd, callbackID);

    return s_callbacks.count(cmd) > 0 && s_callbacks[cmd].count(callbackID) > 0;
}

bool SerialMuxDemux::RemoveCallback(unsigned char cmd)
{
    if(s_callbacks.count(cmd) > 0)
    {
        std::lock_guard<mutex> lockGuard(s_mutex);
        s_callbacks.erase(cmd);
    }

    return true;
}

bool SerialMuxDemux::RemoveCallback(IN unsigned char cmd, const int &callbackID)
{
    if(s_callbacks.count(cmd) > 0)
    {
    	if(s_callbacks[cmd].count(callbackID) > 0)
    	{
    		std::lock_guard<mutex> lockGuard(s_mutex);
    		s_callbacks[cmd].erase(callbackID);

    		if(s_callbacks[cmd].empty())
    		{
    			s_callbacks.erase(cmd);
    		}
    	}
    }

    return true;
}

void SerialMuxDemux::CallbackDemux(const unsigned char *data, const int size)
{
    //TO DO
    if(data != NULL && size > 0)
    {
#ifdef DEBUG
        printf("[serial received]: 0x");
        for(int i = 0; i < size; i++)
        {
            printf("%02X", data[i]);
        }
        printf("\n");
#endif

        if(s_callbacks.count(data[0]) > 0)
        {
        	std::lock_guard<mutex> lockGuard(s_mutex);
        	std::map<int, Callback>::iterator iter = s_callbacks[data[0]].begin();
        	std::map<int, Callback>::iterator endIter = s_callbacks[data[0]].end();

        	for(;iter != endIter; ++iter)
        	{
        		iter->second(data + 1, size - 1);
        	}
        }
    }
}


int SerialMuxDemux::WriteToBuffer(const unsigned char *pBuf, const int dataSize)
{
    //TO DO
    //TO DO
    if(dataSize <= 0)
    {
        return E_SERIAL_NO_DATA;
    }

    //在写入缓冲区之前对缓冲区锁定
    std::lock_guard<std::mutex> lock(_bufMutex);
    //暂存读写标志
    int tWrPos = _wrPos;
    int tRdPos = _rdPos;

    //写入新数据
    int leftCount = SEND_BUFFER_SIZE - _wrPos;    //从写入起点到缓冲区的结尾的字节长度
    if(leftCount > dataSize)
    {
        memcpy(_dataBuf + _wrPos, pBuf, dataSize);
        _wrPos = (_wrPos + dataSize) % SEND_BUFFER_SIZE;   //更新写入起点
    }
    else
    {
        memcpy(_dataBuf + _wrPos, pBuf, leftCount);
        _wrPos = dataSize - leftCount;
        memcpy(_dataBuf, pBuf + leftCount, _wrPos);
    }


    if(_isFull)     //缓冲区满
    {
        //TO DO
        _rdPos = _wrPos;   //更新读出起点
        printf("[%s@SerialMuxDemux]: data buffer for sent to serial is full, some unsent data will be lost\n", __FUNCTION__);
    }
    else if(tWrPos == tRdPos)    //缓冲区空
    {
        //TO DO
        _isFull = (_wrPos == _rdPos);    //更新缓冲区满标志
        _isEmpty = false;
    }
    else if(tWrPos < tRdPos)
    {
        //TO DO
        if(leftCount > dataSize)
        {
            _isFull = _wrPos >= _rdPos;
            _rdPos =  _isFull ? _wrPos : _rdPos;
        }
        else
        {
            _isFull = true;
            _rdPos = _wrPos;

        }
    }
    else if(tWrPos > tRdPos)
    {
        //TO DO
        if(leftCount > dataSize)
        {
            _isFull = false;
        }
        else
        {
            _isFull = _wrPos >= _rdPos;
            _rdPos = _isFull ? _wrPos : _rdPos;
        }
    }


    return dataSize;
}

int SerialMuxDemux::ReadFromBuffer(unsigned char *data, const int size)
{
    //TO DO
    if(NULL == data)
    {
        return E_SERIAL_INVALID_PTR;
    }

    std::lock_guard<std::mutex> lock(_bufMutex);
    if(IsBufferEmpty())   //_wrPos等于_rdPos
    {
        return E_SERIAL_NO_DATA;
    }


    if(_wrPos > _rdPos)    //_wrPos大于_rdPos
    {
        if(_wrPos - _rdPos >= size)
        {
            memcpy(data, _dataBuf + _rdPos, size);
            _rdPos = (_rdPos + size) % SEND_BUFFER_SIZE;
            _isEmpty = (_rdPos == _wrPos);
#ifdef DEBUG
            printf("[%c]write_pos: %d\t read_pos: %d\n", _wrPos >= _rdPos ? 'T' : 'F', _wrPos, _rdPos);
#endif
        }
        else
        {
            return E_SERIAL_NO_DATA;
        }
    }
    else      //_wrPos小于等于_rdPos
    {
        if(_wrPos + SEND_BUFFER_SIZE - _rdPos < size)
        {
            return E_SERIAL_NO_DATA;
        }

        int sizeToBufEnd = SEND_BUFFER_SIZE - _rdPos;
        if(sizeToBufEnd >= size)
        {
            memcpy(data, _dataBuf + _rdPos, size);
            _rdPos = (_rdPos + size) % SEND_BUFFER_SIZE;
            _isEmpty = (_rdPos == _wrPos);

#ifdef DEBUG
            printf("[%c]write_pos: %d\t read_pos: %d\n", _wrPos >= _rdPos ? 'T' : 'F', _wrPos, _rdPos);
#endif
        }
        else
        {
            memcpy(data, _dataBuf + _rdPos, sizeToBufEnd);
            memcpy(data + sizeToBufEnd, _dataBuf, size - sizeToBufEnd);
            _rdPos = size - sizeToBufEnd;
            _isEmpty = (_rdPos == _wrPos);

#ifdef DEBUG
            printf("[%c]write_pos: %d\t read_pos: %d\n", _wrPos >= _rdPos ? 'T' : 'F', _wrPos, _rdPos);
#endif
        }
    }

    if(true == _isFull)
    {
        _isFull = false;
    }
#ifdef DEBUG
    fflush(stdout);
#endif

    return size;
}

void* SerialMuxDemux::SendToSerial(void * arg)
{
    //TO DO
    SerialMuxDemux* sm = static_cast<SerialMuxDemux *>(arg);
    unsigned char sendBuf[SEND_ONCE_SIZE];
#ifdef DEBUG
    printf("SerialMuxDemux发送线程启动\n");
#endif

    int tryCount = 0;
    while(sm->_enableSend)
    {
        if(SEND_ONCE_SIZE == sm->ReadFromBuffer(sendBuf, SEND_ONCE_SIZE))
        {
            //如果当前发送缓冲区中还有未发送的数据
            while(SEND_ONCE_SIZE != sm->_asyncSerial->Send(sendBuf, SEND_ONCE_SIZE) && ++tryCount <= TOTAL_TRY_COUNT)
            {
                //处理发送失败的情况
                std::string failedCmd("");
                for(int iter = 0; iter < SEND_ONCE_SIZE; iter++)
                {
                    failedCmd += sendBuf[iter];
                }

                printf("[failed to send]: %s\n", failedCmd.c_str());
                usleep(35000);
            }
#ifdef DEBUG
            printf("[%lu]发送线程\n", pthread_self());
#endif
            //基于串口数据传输速度限制，在连续发送数据的时候需要短暂休眠防止数据溢出
            usleep(35000);
        }
        else
        {
            //线程轮换调度，避免死循环问题
            usleep(1000);
        }
    }

    return (void *)0;
}


bool SerialMuxDemux::IsBufferEmpty()
{
    //TO DO
    return _isEmpty;
}

bool SerialMuxDemux::IsBufferFull()
{
    //TO DO
    return _isFull;
}


int SerialMuxDemux::GetUsableKey(const std::map<int, Callback>& callbacks)
{
	int i ;

	for(i = 0; ; i++)
	{
		if(0 == callbacks.count(i))
		{
			break;
		}
	}

	return i;
}

bool SerialMuxDemux::ExistCallback(IN const std::map<int, Callback>& callbacks, IN Callback callback, OUT int& callbackID)
{
	callbackID = -1;

	//初始化查询边界
	std::map<int, Callback>::const_iterator iter = callbacks.begin();
	std::map<int, Callback>::const_iterator endIter = callbacks.end();

	//查询指定回调是否存在
	for(;iter != endIter; ++iter)
	{
		if(callback == iter->second)
		{
			callbackID = iter->first;
			return true;
		}
	}

	return false;
}
