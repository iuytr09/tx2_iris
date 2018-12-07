#include <QCoreApplication>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
int main(int argc, char *argv[]) {
    namedWindow( "src", WINDOW_AUTOSIZE );
    Mat src1 = imread( "/root/work/虹膜软件/tx2_iris/image/back1.jpg" );
    while(1) {
        imshow( "src", src1 );
        char c = waitKey(30);
        if( 27==c )
            return 0;
    }
}

