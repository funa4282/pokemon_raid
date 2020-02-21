#include <iostream>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <vector>
#include <string>
#include <fstream>
#include "utils.hpp"
#include "nms.hpp"

//----------------------------
//マクロ

//全体の画像 (探索対象画像) のファイル名
#define TARGET_IMG_FILE ""
//テンプレート画像
#define TEMPLATE_IMG_FILE ""
//結果画像
#define EGG_IMG_FILE ""
#define TIME_IMG_FILE ""

//ウィンドウの名前
//探索対象
#define TARGET_IMG_WINDOW "target"
//テンプレート
#define TEMPLATE_IMG_WINDOW "template"


//----------------------------

int main(int argc, const char * argv[]) {
    
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
    
    double min_val, max_val;    //最小値，最大値
    cv::Point min_loc, max_loc; //最小値の位置，最大値の位置
    //全体画像，テンプレート画像，類似度マップ
    cv::Mat target_img, template_img, compare_img, egg_img, time_img;
    
    //カラーで取得
    target_img = cv::imread(TARGET_IMG_FILE); //全体画像
    template_img = cv::imread(TEMPLATE_IMG_FILE); //テンプレート画像
    if (target_img.empty() || template_img.empty()) { //入力失敗の場合
        fprintf(stderr, "File is not opened.\n");
        return (-1);
    }
    
    //テンプレートマッチング
    cv::matchTemplate(target_img, template_img, compare_img, cv::TM_SQDIFF_NORMED);

    //類似度マップから次の値の取得．最小値，最大値，最小値の位置，最大値の位置
    cv::minMaxLoc(compare_img, &min_val, &max_val, &min_loc, &max_loc);
    
    std::vector<std::vector<float>> rectangles;
    
    //類似度が高いものをrectanglesに格納
    float s; //類似度マップはfloat型
    for (int y=0; y<compare_img.rows; y++) {
        for (int x=0; x<compare_img.cols; x++) {
            s = compare_img.at<float>(y, x); //float型で取得
            if (s < 0.08) { //閾値以下
                std::vector<float> tmp{(float)x, (float)y, (float)x+template_img.cols, (float)y+template_img.rows};
                rectangles.push_back(tmp);
            }
        }
    }
    
    //Non Maximum Suppression で重複矩形削除
    float threshold = 0.5;
    std::vector<cv::Rect> reducedRectangle = nms(rectangles, threshold);
//    DrawRectangles(target_img, reducedRectangle);
    std::cout << reducedRectangle.size() << "\n";
    
    //類似度が最も高い座標を用いて画像の切り抜きを行う（どうやって個別にslackへ送るか）
    for(int i=0; i<reducedRectangle.size(); i++){
//        std::cout << reducedRectangle[i] << " \n";
        
        //egg切り抜き
        cv::Rect egg_size(reducedRectangle[i].x-85, reducedRectangle[i].y-65, reducedRectangle[i].width+100, reducedRectangle[i].height+160);
        egg_img = target_img(egg_size);
        std::string num_egg = std::to_string(i);
        cv::imwrite("" + num_egg + ".jpg", egg_img);

        //time切り抜き
        cv::Rect time_size(reducedRectangle[i].x-85, reducedRectangle[i].y+120, reducedRectangle[i].width+100, reducedRectangle[i].height-60);
        time_img = target_img(time_size);
        std::string num_time = std::to_string(i);
        cv::imwrite("time_" + num_time + ".jpg", time_img);
    }
    
    int flag=0;
    
    //tesseractでレイド時刻の読み取り（どうやって個別にslackへ送るか）
    for(int i=0; i<reducedRectangle.size(); i++){
        
        //レイド場所を取得する
        for(int j=0; j<6; j++){
            std::string raid_num = std::to_string(j);
            std::string raid = "";
            std::string raid_jpg = ".jpg";
            raid = raid + raid_num + raid_jpg;
            
            std::string egg_num = std::to_string(i);
            std::string egg = "";
            std::string egg_jpg = ".jpg";
            egg = egg + egg_num + egg_jpg;
            //カラーで取得
            target_img = cv::imread(egg); //全体画像
            template_img = cv::imread(raid); //テンプレート画像
            if (target_img.empty() || template_img.empty()) { //入力失敗の場合
                fprintf(stderr, "File is not opened.\n");
                return (-1);
            }
            //テンプレートマッチング
            cv::matchTemplate(target_img, template_img, compare_img, cv::TM_SQDIFF_NORMED);
            
            //類似度マップから次の値の取得．最小値，最大値，最小値の位置，最大値の位置
            cv::minMaxLoc(compare_img, &min_val, &max_val, &min_loc, &max_loc);
            
            float s; //類似度マップはfloat型
            for (int y=0; y<compare_img.rows; y++) {
                for (int x=0; x<compare_img.cols; x++) {
                    s = compare_img.at<float>(y, x); //float型で取得
                    if (s < 0.01) { //閾値以下
                        if(j==0){
                            printf("瀬戸万博記念公園で");
                            flag = 1;
                            break;
                        }else if(j==1){
                            printf("天水皿で");
                            flag = 1;
                            break;
                        }else if(j==2){
                            printf("椀貸池で");
                            flag = 1;
                            break;
                        }else if(j==3){
                            printf("計算センターで");
                            flag = 1;
                            break;
                        }else if(j==4){
                            printf("貯水タワーで");
                            flag = 1;
                            break;
                        }else if(j==5){
                            printf("Sign in AITで");
                            flag = 1;
                            break;
                        }
                    }
                }
                if(flag==1){
                    flag = 0;
                    break;
                }
            }
        }
        
        char *outText;
        tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
        // Initialize tesseract-ocr with English, without specifying tessdata path
        if (api->Init(NULL, "eng")) {
            fprintf(stderr, "Could not initialize tesseract.\n");
            exit(1);
        }
    
        // Open input image with leptonica library
        std::string time_num = std::to_string(i);
        std::string time = "";
        std::string time_jpg = ".jpg";
        time = time + time_num + time_jpg;
        Pix *image = pixRead(time.c_str());
        api->SetImage(image);
        // Get OCR result
        outText = api->GetUTF8Text();
        //    printf("OCR output:\n%s", outText);
        printf("%s", outText);
        
        //~xx:xx -> xx:xx
        const char *cstr = outText;
        std::string str = std::string(cstr);
        str.erase(str.end()-2); //end()は最後の次を指す
    
        // Destroy used object and release memory
        api->End();
        delete [] outText;
        pixDestroy(&image);
    }
    
    return 0;
}
