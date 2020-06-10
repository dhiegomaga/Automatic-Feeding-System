//
// Author : D Bersan
//

/*
 * Load model saved using tensorflow SavedModel format.
 *  
 * To access the names of the input and output tensors of the model, you can use the `saved_model_cli` tool, inside python bin/ folder. 
 * 
 * The syntax is (for the default tag-set and signature): `saved_model_cli show --dir /path/to/saved_model_folder/ --tag_set serve --signature_def serving_default`
 * 
 * > TODO Fix code to use other SignatureDefs other than the default one (It is also possible to define the name of the tensors via code while saving the model, but the documentation isn't so clear). 
 * 
 * More info at: https://stackoverflow.com/questions/58968918/accessing-input-and-output-tensors-of-a-tensorflow-2-0-savedmodel-via-the-c-api?noredirect=1#comment109422705_58968918
*/

#include "include/Model.h"
#include "include/Tensor.h"
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <ctime> 
#include <iostream>

using namespace cv;

int main() {

    std::cout<<"Initializing with TF version "<< TF_Version() << std::endl;

    VideoCapture cap;
    if(!cap.open(0))
        return 0;

    auto last_time = std::chrono::system_clock::now();
    auto second = std::chrono::duration<double>(1.0f);

    while(true){
        Mat frame;
        cap >> frame;
        if( frame.empty() ) break; // end of video stream
        
        // Get fps
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = now-last_time;
        double fps = (second/elapsed_seconds);
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << fps;
        std::string fps_str = stream.str();
        last_time = now;

        // Put text
        std::string text = fps_str;
        cv::putText(frame, 
            text,
            cv::Point(10,450), // Coordinates
            cv::FONT_HERSHEY_SIMPLEX, // Font
            1.0, // Scale. 2.0 = 2x bigger
            cv::Scalar(20,200,10), // BGR Color
            2, // Line Thickness (Optional)
            2); // Anti-alias (Optional)

        // Show image
        imshow( "Frame", frame );

        if( waitKey(1) == 27 ) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;


/*
    // Load model saved using the SavedModel format from tensorflow 2.0
    Model m("../model");

    // Create Tensors
    Tensor input(m, "serving_default_graph_input");
    Tensor prediction(m, "StatefulPartitionedCall");

    // Read image
    for (int i=1; i<=5; i++) {
        cv::Mat img, scaled;

        // Read image
        img = cv::imread("../images/"+std::to_string(i)+".jpg");

        if(img.dims == 0){
            std::cout<<"Image not found. "<<std::endl;
            return -1;
        }
        
        // Scale image to range 0-1
        img.convertTo(scaled, CV_64F, 1.f/255);

        int rows = img.rows;
        int cols = img.cols;

        // Put data inside vector
        std::vector<double> img_data;
        img_data.assign(scaled.begin<double>(), scaled.end<double>());
        
        // Feed data to input tensor
        input.set_data(img_data, {1, rows, cols});
        
        // Run and show predictions
        m.run(input, prediction);
        
        // Get tensor with predictions
        auto result = prediction.Tensor::get_data<double>();
        
        // Maximum prob
        auto max_result = std::max_element(result.begin(), result.end());
        
        std::cout << "Real label: " << i << ", predicted: " << std::distance(result.begin(), max_result) << ", Probability: " << (*max_result) << std::endl;

    }

*/
}
