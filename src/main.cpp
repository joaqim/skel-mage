
//#include "impl/delay.hpp"

#include "SavefileManager.cpp"
#include "impl/delay.hpp"

#include "MouseInput.cpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>


//#include "ScreenShot.cpp"

cv::Rect findMatch(cv::Mat &img, cv::Mat const &templ, cv::Mat const &mask) {
  using namespace cv;

  // Match template
  Mat1f matrix;

  //matchTemplate(img, templ, matrix, TM_CCOEFF_NORMED);
  matchTemplate(img, templ, matrix, TM_CCORR_NORMED, mask);
  // matchTemplateMask( InputArray _img, InputArray _templ, OutputArray _result, int method, InputArray _mask )

  // Find highest response
  Point maxLoc;
#if 0 // No evaluation
  minMaxLoc(matrix, NULL, NULL, NULL, &maxLoc);
#else
  //double minVal; double maxVal; Point minLoc; Point maxLoc;
  //minMaxLoc( matrix, &minVal, &maxVal, &minLoc, &maxLoc);

  double maxVal;
  minMaxLoc( matrix, NULL, &maxVal, NULL, &maxLoc);

  if (maxVal < 0.80)
  {
    // No match
    return Rect{0,0,0,0};
  }

  int intensity{200}; // Looking for < 90

  if (maxLoc.x+6 <= matrix.size().width || maxLoc.y+6 <= matrix.size().height) {
    intensity = (int)matrix.at<uchar>(maxLoc.x+6,maxLoc.y+6);
    std::cout << "intensity: " << intensity << "\n";
}

  if (intensity > 90) {
    // Erase detected rect
    rectangle(img, {maxLoc, templ.size()}, Scalar(0,0,0), CV_FILLED);
    // Try again

    std::cout << "Trying again\n";
    return findMatch(img, templ, mask);
  }

#endif
  return Rect(maxLoc, templ.size());
};
#include <time.h>

#if 1
int main() {
  using namespace cv;

#if 0
  {
    std::vector<std::string>testImages = {
      "../../../2018-11-25-143136_1920x1200_scrot.png",
      "../../../2018-11-25-143531_1920x1200_scrot.png",
      "../../../2018-11-25-143537_1920x1200_scrot.png",
      "../../../2018-11-25-143538_1920x1200_scrot.png",
      "../../../2018-11-25-143539_1920x1200_scrot.png",
      "../../../2018-11-25-144200_1920x1200_scrot.png"
    };

    // Load template
    Mat3b templ = imread("../misc/icon_1.png");
    Mat1b templ_gray;
    cvtColor(templ, templ_gray, COLOR_BGR2GRAY);

    for(auto const &img_path : testImages) {
      // Load image
      //Mat3b img = imread("../../../2018-11-25-143136_1920x1200_scrot.png");
      Mat3b img = imread(img_path);
      Mat1b img_gray;
      cvtColor(img, img_gray, COLOR_BGR2GRAY);

      // Convert to grayscale
      auto rect = findMatch(img_gray, templ_gray, mask);

      if(rect.empty()) {
        std::cout << img_path << "\n";
        return 1;
      }

      rectangle(img, rect, Scalar(0,0,255), 2);
      imshow("Result", img);
      waitKey();
    }
  }
#else
  // Load template
  Mat3b templ = imread("../misc/icon_3.png");
  Mat1b templ_gray;

  assert(!templ.empty());
  cvtColor(templ, templ_gray, COLOR_BGR2GRAY);
  //templ_gray = templ;

  Mat1b mask = imread("../misc/icon_mask.png", IMREAD_GRAYSCALE);
  //bitwise_not(mask, mask);
  //imshow("mask", mask);
  //waitKey();

#if 1
  auto screen = ScreenShot(630, 1006, 64*10, 64);
  Mat screen_img;
  screen(screen_img);
  imwrite("screen_img.png", screen_img);
#elif 0
  Mat3b screen_img = imread("screen_gray.png");
#else
  Mat3b img = imread("../../../2018-11-25-143136_1920x1200_scrot.png");
  Mat3b screen_img = img(Rect{630, 1006, 64*10, 64});
#endif

  Mat1b screen_gray;
  assert(!screen_img.empty());
  cvtColor(screen_img, screen_gray, COLOR_BGR2GRAY);
  //screen_gray = screen_img;
  //cvtColor(screen_img, screen_gray, COLOR_RGBA2GRAY);
  imwrite("screen_gray.png", screen_gray);

  auto rect = findMatch(screen_gray, templ_gray, mask);

  if(rect.empty() || rect.x <= 2 && rect.y != 4) {
    std::cout << "Failed rect: " << rect << "\n";
    auto result = std::time(nullptr);
    //std::cout << std::asctime(std::localtime(&result)) << "\n";
    imwrite("./output/"+ std::to_string(result) + "-fail.png", screen_gray);
    return 1;
  }

  std::cout << rect.area() << "\n";

#if 0
  rectangle(screen_img, rect, Scalar(0,0,255), 2);
  imshow("Result", screen_img);
  waitKey();
#elif 0
  rectangle(screen_gray, rect, Scalar(0,0,255), 2);
  auto result = std::time(nullptr);
  //std::cout << std::asctime(std::localtime(&result)) << "\n";
  imwrite("./output/"+ std::to_string(result) + ".png", screen_gray);
#endif

OCR ocr;
auto cropRect = Rect{rect.x+34,
                     rect.y+32,
                     rect.width-34-5,
                     rect.height-32-8};

std::cout << rect << "\n";
std::string text =  ocr.recognize(screen_img(cropRect)).c_str();
printf("Recognized text: %s \n", text.c_str());

int num{0};
for(char const &c : text) {
  if(isdigit(c)) {
    num = atoi(&c);
    break;
  }
  if(c == 'D') num = 6;
  if(c == 'Q') num = 9;
  if(c == 'a') num = 4;
}

if(num == 9) { num=2;}
if(num == 92) { num=2;}
if(num == 29) { num=2;}
if(num == 0) { num=10;}
if(num == 1) { num=10;}

//MouseInput mouse;
//if(num < 10) mouse.click(100,100);

auto result = std::time(nullptr);
//std::cout << std::asctime(std::localtime(&result)) << "\n";
imwrite("./output/"+ std::to_string(result) + "-" + text + "_"+std::to_string(num)+ ".png", screen_gray(cropRect));

if(num%2 != 0) return 1;

if(num < 10) return 0;

//Mat croppedImg = screen_img(rect);
//printf("Recognized text: %s \n", ocr.recognize(croppedImg).c_str());

#endif

               return 0;
}
#endif

#if 0
int main() {
#if 0
  SpellCheck spellcheck;

  std::string text = "12 December 1444";

  //TODO: Add whitespace between number and string if missing 12December1444 -> 12 December 1444
  auto vDate = spellcheck(text);

  if(vDate.size() > 0) {
    std::cout << joinStringVector(vDate, "_") << "\n";
  }

  ScreenShot screen(1706, 16, 116, 19);
  cv::Mat image;
  screen(image);
  cv::imshow("image", image);
  cv::waitKey(0);
#endif

#if 0
  auto save = Savefile("Test.tmp");
  save.date = "1444_11_11";
  std::cout << save << std::endl;
  impl::makePath("./"+ save.name); 
  assert(impl::directoryExist("./"+ save.name)); 

  std::fstream file_in{"text.txt", std::ios::in};
  Savefile save_in;
  std::set<Savefile> savefiles;
  while(!file_in.eof()) {
    std::string line;
    std::getline(file_in, line);
    if(line.size() > 4) {
      line >> save_in;
      savefiles.insert(save_in);
    }
  }
  file_in.close();

#if 0
  std::ofstream file_out;
  file_out.open("text.txt", std::ios::out | std::ios::trunc);
  for(auto const save : savefiles) {
    file_out << save << std::endl;
  }
  file_out.close();
#endif


  std::cout << save_in << std::endl;
  assert(save_in == save);

  return 0;
#endif
  SavefileManager savefileManager;
  for(;;) {
    savefileManager.update();
    //impl::delayMS(50);
  }

return 0;
}
#endif

