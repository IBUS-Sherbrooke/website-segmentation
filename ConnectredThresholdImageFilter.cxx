#include "itkConnectedThresholdImageFilter.h"
#include "itkImage.h"
#include "itkCastImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
int
main(int argc, char * argv[])
{
  std::cout << "Deeeeeebug world!2" << std::endl;
  std::cout << argv[0] << std::endl;
  std::cout << argv[1] << std::endl;
  std::cout << argv[2] << std::endl;
  std::cout << argv[3] << std::endl;
  std::cout << argv[4] << std::endl;
  std::cout << argv[5] << std::endl;
  std::cout << argv[6] << std::endl;
  std::cout << argv[7] << std::endl;

  if (argc < 8)
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr
      << " inputImage  outputImage seedX seedY seedZ lowerThreshold upperThreshold"
      << std::endl;
    return EXIT_FAILURE;
  }

  using InternalPixelType = float;
  constexpr unsigned int Dimension = 3;
  using InternalImageType = itk::Image<InternalPixelType, Dimension>;

  using NamesGeneratorType = itk::GDCMSeriesFileNames;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

  nameGenerator->SetUseSeriesDetails(true);
  nameGenerator->AddSeriesRestriction("0008|0021");
  nameGenerator->SetGlobalWarningDisplay(false);
  nameGenerator->SetDirectory(argv[1]);
  using SeriesIdContainer = std::vector<std::string>;
  const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
  auto                      seriesItr = seriesUID.begin();
  auto                      seriesEnd = seriesUID.end();



  std::string seriesIdentifier;
  while (seriesItr != seriesUID.end()){
    seriesIdentifier = seriesItr->c_str();
    seriesItr++;
}
  std::cout << "\nReading: ";
  std::cout << seriesIdentifier << std::endl;
  using FileNamesContainer = std::vector<std::string>;
  FileNamesContainer fileNames =
    nameGenerator->GetFileNames(seriesIdentifier);

  using ReaderType = itk::ImageSeriesReader<InternalImageType>;
  ReaderType::Pointer rdr = ReaderType::New();
  using ImageIOType = itk::GDCMImageIO;
  ImageIOType::Pointer dicomIO = ImageIOType::New();
  rdr->SetImageIO(dicomIO);
  rdr->SetFileNames(fileNames);
  rdr->ForceOrthogonalDirectionOff(); // properly read CTs with gantry tilt

  using WriterType = itk::ImageFileWriter<InternalImageType>;
  WriterType::Pointer writ = WriterType::New();

  writ->SetFileName(argv[2]);
  writ->UseCompressionOn();
  writ->SetInput(rdr->GetOutput());

  using OutputPixelType = float;
  using OutputImageType = itk::Image<OutputPixelType, Dimension>;
  using CastingFilterType =
    itk::CastImageFilter<InternalImageType, OutputImageType>;
  CastingFilterType::Pointer caster = CastingFilterType::New();

  using WriterType = itk::ImageFileWriter<OutputImageType>;

  WriterType::Pointer writer = WriterType::New();

  writer->SetFileName(argv[2]);

  using CurvatureFlowImageFilterType =
    itk::CurvatureFlowImageFilter<InternalImageType, InternalImageType>;



  CurvatureFlowImageFilterType::Pointer smoothing =
    CurvatureFlowImageFilterType::New();

  using ConnectedFilterType =
  itk::ConnectedThresholdImageFilter<InternalImageType, InternalImageType>;

  ConnectedFilterType::Pointer connectedThreshold =
  ConnectedFilterType::New();

  smoothing->SetInput(rdr->GetOutput());
  connectedThreshold->SetInput(smoothing->GetOutput());
  caster->SetInput(connectedThreshold->GetOutput());

  //// Software Guide : EndCodeSnippet
  using ImageType = itk::Image<float, 3>;
  using RdrType = itk::ImageFileReader<ImageType>;

  ImageType::Pointer mask = ImageType::New();
  using MaskFilterType = itk::MaskImageFilter<ImageType, ImageType>;
  MaskFilterType::Pointer maskFilter = MaskFilterType::New();
  maskFilter->SetInput(rdr->GetOutput());
  maskFilter->SetMaskImage(caster->GetOutput());

  writ->SetInput(maskFilter->GetOutput());


  smoothing->SetNumberOfIterations(5);
  smoothing->SetTimeStep(0.125);
  const InternalPixelType lowerThreshold = std::stod(argv[6]);
  const InternalPixelType upperThreshold = std::stod(argv[7]);

  connectedThreshold->SetLower(lowerThreshold);
  connectedThreshold->SetUpper(upperThreshold);
  connectedThreshold->SetReplaceValue(255);
  InternalImageType::IndexType index;
  index[0] = std::stoi(argv[3]);
  index[1] = std::stoi(argv[4]);
  index[2] = std::stoi(argv[5]);
  connectedThreshold->SetSeed(index);

  try
  {
    writ->Update();
  }
  catch (const itk::ExceptionObject & excep)
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }

  std::cout << "Done!";

  return EXIT_SUCCESS;
}
