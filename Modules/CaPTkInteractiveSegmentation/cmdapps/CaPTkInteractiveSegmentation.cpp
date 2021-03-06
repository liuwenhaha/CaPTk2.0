#include <mitkCommandLineParser.h>
#include <mitkIOUtil.h>
#include <mitkDataStorage.h>
#include <mitkStandaloneDataStorage.h>
#include <mitkImageCast.h>

#include <CaPTkInteractiveSegmentation.h>
#include <GeodesicTrainingSegmentation.h>

// #include <json/json.h>

#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>

/**Splits a string into a list using a delimiter*/
std::vector<std::string> 
split(const std::string &s, char delim) {
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(std::move(item));
	}
	return elems;
}

/** \brief command-line app for batch processing of images
 *
 * This command-line app takes a task and and a cohort and runs the algorithm on everything.
 */
int main(int argc, char* argv[])
{
  mitkCommandLineParser parser;

  /**** Set general information about the command-line app ****/

  parser.setCategory("CaPTk Cmd App Category");
  parser.setTitle("CaPTk Interactive Segmentation Cmd App");
  parser.setContributor("CBICA");
  parser.setDescription(
    "This command-line app takes a task and cohort and runs the interactive segmentation algorithm on everything.");

  // How should arguments be prefixed
  parser.setArgumentPrefix("--", "-");

  /**** Add arguments. Unless specified otherwise, each argument is optional.
            See mitkCommandLineParser::addArgument() for more information. ****/

  parser.addArgument(
    "images",
    "i",
    mitkCommandLineParser::String,
    "Images",
    "Paths to the input images, separated by comma",
    us::Any(),
    false);

  parser.addArgument(
    "labels",
    "l",
    mitkCommandLineParser::String,
    "Labels Image",
    "Path to the input seeds image.",
    us::Any(),
    false);

  parser.addArgument(
    "priorimages",
    "p",
    mitkCommandLineParser::String,
    "Prior Images",
    "These are also input images, but are generated by a deep learning algorithm",
    us::Any(),
    true);

  parser.addArgument(
    "outputdir",
    "o",
    mitkCommandLineParser::String,
    "Output Directory",
    "Where to generate the output",
    us::Any(),
    false);

  // // Add arguments. Unless specified otherwise, each argument is optional.
  // // See mitkCommandLineParser::addArgument() for more information.
  // parser.addArgument(
  //   "task",
  //   "t",
  //   mitkCommandLineParser::String,
  //   "Task",
  //   "JSON file that contains information on how to run this execution.",
  //   us::Any(),
  //   false);
  // parser.addArgument(
  //   "cohort",
  //   "c",
  //   mitkCommandLineParser::String,
  //   "Cohort",
  //   "JSON file that contains information on how to run this execution.",
  //   us::Any(),
  //   false);

  /**** Parse arguments. This method returns a mapping of long argument names to
            their values. ****/

  auto parsedArgs = parser.parseArguments(argc, argv);

  if (parsedArgs.empty())
    return EXIT_FAILURE; // Just exit, usage information was already printed.

  if (parsedArgs["task"].Empty() || parsedArgs["cohort"].Empty())
  {
    MITK_INFO << parser.helpText();
    return EXIT_FAILURE;
  }

  // // Parse, cast and set required arguments
  // auto task   = us::any_cast<std::string>(parsedArgs["task"]);
  // auto cohort = us::any_cast<std::string>(parsedArgs["cohort"]);

  auto imagesPaths = us::any_cast<std::string>(parsedArgs["images"]);
  auto labelsPath  = us::any_cast<std::string>(parsedArgs["labels"]);
  auto outputDir   = us::any_cast<std::string>(parsedArgs["outputdir"]);

  /**** Default values for optional arguments ****/

  std::string priorImagesPaths = "";
  // // Parse, cast and set optional arguments
  if (parsedArgs.end() != parsedArgs.find("priorimages"))
  {
    priorImagesPaths = us::any_cast<std::string>(parsedArgs["priorimages"]);
  }

  std::vector<std::string> imagesPathsVector = split(imagesPaths, ',');
  std::vector<std::string> priorImagesPathsVector = split(priorImagesPaths, ',');

  /**** Run ****/

  try
  {
    std::vector<mitk::Image::Pointer> images;
    std::vector<mitk::Image::Pointer> priorImages;
    mitk::LabelSetImage::Pointer      seeds;

    /**** Read input ****/

    for (std::string& imagePath : imagesPathsVector)
    {
      auto image = mitk::IOUtil::Load<mitk::Image>(imagePath);
      images.push_back(image);
    }

    for (auto& priorImagePath : priorImagesPathsVector)
    {
      auto priorImage = mitk::IOUtil::Load<mitk::Image>(priorImagePath);
      priorImages.push_back(priorImage);
    }

    seeds = mitk::IOUtil::Load<mitk::LabelSetImage>(labelsPath);

    // auto algorithm = new CaPTkInteractiveSegmentation(nullptr);
    // algorithm->Run(task, cohort);
    
    if (images[0]->GetDimension() == 3)
    {
      // [ 3D ]

      /**** Convert to itk ****/

      std::vector<itk::Image<float,3>::Pointer> imagesItk;
      std::vector<itk::Image<float,3>::Pointer> priorImagesItk;
      typename itk::Image<int,3>::Pointer       seedsItk;

      for (auto& image : images)
      {
        typename itk::Image<float, 3>::Pointer imageItk;
        mitk::CastToItkImage(image, imageItk);
        imagesItk.push_back(imageItk);
      }

      for (auto& image : priorImages)
      {
        typename itk::Image<float, 3>::Pointer imageItk;
        mitk::CastToItkImage(image, imageItk);
        priorImagesItk.push_back(imageItk);
      }

      mitk::CastToItkImage(seeds, seedsItk);

      std::unique_ptr<GeodesicTrainingSegmentation::Coordinator<float,3>> geodesicTraining(
        new GeodesicTrainingSegmentation::Coordinator<float,3>()
      );
      geodesicTraining->SetInputImages(imagesItk);
      geodesicTraining->SetExtraInputImagesNotAGDable(priorImagesItk);
      geodesicTraining->SetLabels(seedsItk);
      geodesicTraining->SetOutputPath(outputDir);
      // geodesicTraining->SetNumberOfThreads(16);
      // geodesicTraining->SaveOnlyNormalSegmentation(true, "segmentation");
      geodesicTraining->SetVerbose(true);

      /**** Run algorithm ****/

     auto result = geodesicTraining->Execute();

      if (result->ok)
      {
          mitk::Image::Pointer segmNormal;
          mitk::CastToMitkImage(result->labelsImage, segmNormal);

          mitk::LabelSetImage::Pointer segm = mitk::LabelSetImage::New();
          
          segm->InitializeByLabeledImage(segmNormal);

          mitk::IOUtil::Save(segm, outputDir + std::string("/segmentation.nii.gz"));
      }
      else {
          std::cout << "Algorithm finished with internal error\n";
          return EXIT_FAILURE;
      }
    }
    else
    {
      // [ 2D ]

      std::cout << "2D not supported yet\n";
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }
  catch (const std::exception &e)
  {
    MITK_ERROR << e.what();
    return EXIT_FAILURE;
  }
  catch (...)
  {
    MITK_ERROR << "Unexpected error!";
    return EXIT_FAILURE;
  }
}
