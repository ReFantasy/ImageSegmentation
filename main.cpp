#include <iostream>
#include "Imagesplit.h"
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <map>
using namespace std;

//read command line param
map<string, string> readCmdParam(int argc,char *argv[]);

//get image file path and name
vector<string> getFilePathAndName(const string &image_file);

//remove the point "." in file name to get the dir which to save the images proceed
string getImageDir(const string &image_name);

int main(int argc, char *argv[])
{
    //read param
    map<string, string> map_cmd_param = readCmdParam(argc, argv);

    //get the image file
    string image_file(map_cmd_param["-file"]);

    //get the image path and name 
    auto vecstr = getFilePathAndName(image_file);
    string image_path = vecstr[0];
    string image_name = vecstr[1];
    if(image_name.empty())
        return -1;

    //create images dir
    string dir_name = getImageDir(image_name);
    string dir_path = image_path;
    //cout<<string(dir_path+dir_name)<<endl;
    bool isDirCreated = mkdir(string(dir_path+dir_name).c_str(), S_IRWXU|S_IRWXG);
    if(isDirCreated)
        cout<<"create dir failed"<<endl;

    //create class instance
    int threshold = atoi(map_cmd_param["-threshold"].c_str());
    int facecount = atoi(map_cmd_param["-facecount"].c_str());
    int minimumsize =atoi(map_cmd_param["-minimumsize"].c_str());
    //cout<<"threshold "<<threshold<<endl<<"facecount "<<facecount<<endl<<"minimumsize "<<minimumsize<<endl;

    Imagesplit plitimg(threshold, facecount, minimumsize);

    plitimg.loadImage(image_file);

    bool res = plitimg.setClassifier("haarcascade_frontalface_alt2.xml");
    if(!res)
    {
        cout<<"set classifier failed."<<endl;
        return -1;
    }

    //process image

    bool isProceed = plitimg.processImage(string(dir_path+dir_name+"/"));
    if(isProceed)
        cout<<"Complete!"<<endl;
    else
        cout<<"Failure!"<<endl;


    //Mat img = imread(image_file);
    //imshow("1", img);
    //waitKey(0);
    return 0;
}

map<string, string> readCmdParam(int argc, char *argv[])
{
    //default setting
    map<string, string> mapcmdparam;
    mapcmdparam["-file"] = "";
    mapcmdparam["-threshold"] = "160";
    mapcmdparam["-facecount"] = "8";
    mapcmdparam["-minimumsize"] = "195";

    //read command line param
    for(int i=1; i<argc-1; i++)
    {
         string p1(argv[i]);
         string p2(argv[i+1]);
         if(p1[0]=='-' && p2[0]!='-')
         {
             mapcmdparam[p1] = p2;
         }
    }

    return mapcmdparam;
}

vector<string> getFilePathAndName(const string &image_file)
{
    int index = image_file.length()-1;
    for(;index>=0 && image_file[index]!='/'; index--);
    string image_path;
    string image_name;

    if(index<0)
    {
        //get the path
        image_path = "./";

        //get the name
        image_name = image_file;
       

        
    }
    else
    {
        //get the path
        image_path = image_file.substr(0, ++index);

        //get the name
        image_name = image_file.substr(index);

    }

    if(image_path == string("./"))
    {
        char absoulte_path[1024];   
        getcwd(absoulte_path, 1024);

        image_path = string(absoulte_path+string("/"));
    }
    //return
    return vector<string>({image_path, image_name});
   
}

string getImageDir(const string &image_name)
{
    string image_dir = image_name;

    //remove the point in image_dir
    for(string::iterator iter = image_dir.begin(); iter != image_dir.end(); )
    {
        if(*iter == '.')
        {
            image_dir.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    return image_dir;
}


















