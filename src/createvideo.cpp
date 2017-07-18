//============================================================================
// Name        : poylon2opencv.cpp
// Author      : Joseph J. Gunn
// Version     :
// Copyright   : copyleft
// Description : POC for vision label reader
//============================================================================

#include <getopt.h>
#include <string>
#include <time.h>
#include <signal.h>

#include <iostream>
#include <fstream>
using namespace std;

// Include files to use the PYLON API.
#include <pylon/PylonIncludes.h>
#include <pylon/PylonImage.h>
#include <pylon/ImageFormatConverter.h>

// opencv includes
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <tesseract/baseapi.h>
#include <zbar.h>

static const size_t c_maxCamerasToUse = 2;
static const int framesToAverage = 5;

static bool _sig_running = false;
void _sig_cb( int sig) {
    _sig_running = false;
}

bool mkpath( std::string path )
{
    bool bSuccess = false;
    int nRC = ::mkdir( path.c_str(), 0775 );
    if( nRC == -1 )
    {
        switch( errno )
        {
            case ENOENT:
                //parent didn't exist, try to create it
                if( mkpath( path.substr(0, path.find_last_of('/')) ) )
                    //Now, try to create again.
                    bSuccess = 0 == ::mkdir( path.c_str(), 0775 );
                else
                    bSuccess = false;
                break;
            case EEXIST:
                //Done!
                bSuccess = true;
                break;
            default:
                bSuccess = false;
                break;
        }
    }
    else
        bSuccess = true;
    return bSuccess;
}

void pylonSave( Pylon::CGrabResultPtr ptrGrabResult, const string &base, const string &fname) {
static int i = 0;
    char p[MAX_PATH] = "";
    i++;
    mkpath(base);

    snprintf( p, MAX_PATH, "%s/%s%04d.png", base.c_str(), fname.c_str(), i);
    Pylon::CImagePersistence::Save( Pylon::ImageFileFormat_Png, p, ptrGrabResult);

}

void saveRaw( Pylon::CGrabResultPtr ptrGrabResult, const string &base, const string &fname) {
static int i = 0;
    char p[MAX_PATH] = "";
    i++;
    mkpath(base);

    snprintf( p, MAX_PATH, "%s/%s%04d.raw", base.c_str(), fname.c_str(), i);
    ofstream fd;
    fd.open( p,ios::out | ios::binary);
    if (fd.is_open()) {
        fd.write( (const char *)ptrGrabResult->GetBuffer(), ptrGrabResult->GetPayloadSize());
    }
    fd.close();

}

void opencvSave( const cv::Mat &image, const string &base, const string &fname) {
static int i = 0;
    char p[MAX_PATH] = "";
    i++;
    mkpath(base);

    snprintf( p, MAX_PATH, "%s/%s%04d.png", base.c_str(), fname.c_str(), i);
    cv::imwrite( p, image );

}

class convertException: public exception
{
private:
    const char *estr;
public:
    convertException( const char *e) :estr(e) {};
  virtual const char* what() const throw()
  {
    return estr;
  }
};


cv::Mat pylon2Mat( Pylon::IImage &img) {
    cv::Mat m;
    if ( img.GetPixelType() == Pylon::PixelType_BayerBG8 ) {
            return    cv::Mat(
                  img.GetHeight(),
                  img.GetWidth(),
                  CV_8UC1,
                  img.GetBuffer(),
                  cv::Mat::AUTO_STEP
                  );
    } else if ( img.GetPixelType() == Pylon::PixelType_Mono8 ) {
            return    cv::Mat(
                  img.GetHeight(),
                  img.GetWidth(),
                  CV_8UC1,
                  img.GetBuffer(),
                  cv::Mat::AUTO_STEP
                  );
    }
    throw convertException( "Unsupported pixel type");
}

int main(int argc, char* argv[]) {
    Pylon::PylonAutoInitTerm autoInitTerm;
    string file = "video.avi";
    int c;
    int i = 0;
    bool writeFrames = false;
    bool rawFrames = false;
    bool savePylon = false;
    opterr = 0;

    while ((c = getopt (argc, argv, "uprf:")) != -1)
      switch (c)
    {
        case 'f':
            file = optarg;
            break;
        case 'u':
            writeFrames = true;
            break;
        case 'p':
            savePylon = true;
            break;
        case 'r':
            rawFrames = true;
            break;
        case '?':
            if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
            return 1;
    }


    try
    {
        cv::VideoWriter writter;

        // Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
        Pylon::CInstantCamera camera( Pylon::CTlFactory::GetInstance().CreateFirstDevice());

        // Print the model name of the camera.
        cout << "Using device " << camera.GetDeviceInfo().GetModelName(); cout << endl;

        camera.MaxNumBuffer = 5;

            // start the camera up
        camera.StartGrabbing();

            // This smart pointer will receive the grab result data.
        Pylon::CGrabResultPtr ptrGrabResult;

            // Grab images from the camera.
        int key = -1; // No key pressed
        _sig_running = true;
        signal( SIGINT, _sig_cb);
        while ( camera.IsGrabbing()) {
            camera.RetrieveResult( 5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

            if (ptrGrabResult->GrabSucceeded())
            {
                if ( rawFrames) {
                    saveRaw( ptrGrabResult, "video", file);
                }
                if ( savePylon) {
                    pylonSave( ptrGrabResult, "video", file);
                } else {


                    try {
                        i++;
                        cv::Mat image;
                        image = pylon2Mat( ptrGrabResult);
                        if ( writeFrames ) {
                            opencvSave( image, "video", file);
                        } else {
                            if ( !writter.isOpened())
                                if (writter.open( file, cv::VideoWriter::fourcc('F','F','V','1'), 10, image.size(), true)) {
                                    cout << "Must have failed on open of " << file << endl;
                                    break;
                                }
                                if (writter.isOpened()) {
                                    writter.write(image);
                                }
                        }
                        cv::imshow( "Captured", image );
                    } catch (convertException e) {
                        cout << e.what() << endl; 
                    }
                    key = cv::waitKey(1);
                    if ( key == 27)
                        break;
                }
            }
            if (!_sig_running)
                break;
        }
    }
    catch (GenICam::GenericException &e)
    {
            // Error handling
        cerr << "An exception occurred." << endl
        << e.GetDescription();cout << endl;
    }


    return 0;
}
