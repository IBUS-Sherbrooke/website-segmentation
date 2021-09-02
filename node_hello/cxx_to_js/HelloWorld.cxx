#include "itkConnectedThresholdImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkIndex.h"
#include <itkMedianImageFilter.h>
#include <itkMacro.h>
//#include <itkCastImageFilter.h>
int main(int argc, char * argv[]) {
	std::cout << "Debug world!" << std::endl;
	if( argc < 9 )
    {
 	std::cerr << "Usage: " << argv[0] << " medianfilter <inputImage> <outputImageFile> <lowerThreshold> <upperThreshold> <indexx>  <indexy> <indexz>" << std::endl;
    return EXIT_FAILURE;
	}
	//assignation des var argv
	const char * inputImageFile = argv[2];
  	const char * outputImageFile = argv[3];
  	unsigned int lowerThreshold = atoi( argv[4] );
	unsigned int upperThreshold = atoi( argv[5] );
	unsigned int indexx = atoi( argv[6] );
	unsigned int indexy = atoi( argv[7] );
	unsigned int indexz = atoi( argv[8] );
	// Declaration des parametres de l'image 
	using InternalPixelType = float;
	constexpr unsigned int Dimension = 3;
	using InternalImageType = itk::Image<InternalPixelType, Dimension>;
	using ReaderType = itk::ImageFileReader< InternalImageType >;
  	auto reader = ReaderType::New();
  	reader->SetFileName( inputImageFile );
	
	//setup du pipeline de smoothing
	using CurvatureFlowImageFilterType = itk::CurvatureFlowImageFilter<InternalImageType, InternalImageType>;
	CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();
	smoothing->SetInput(reader->GetOutput()); //Valeur en input
	smoothing->SetNumberOfIterations(5);
	smoothing->SetTimeStep(0.125);

	// Index de depart pour segmentation
	using IndexType = itk::Index<Dimension>;
  	IndexType index;
	index[0] = indexx;
  	index[1] = indexy;
	index[2] = indexz;

	// setup du pipeline de segmentation
	using ConnectedFilterType = itk::ConnectedThresholdImageFilter<InternalImageType, InternalImageType>;
	ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
	connectedThreshold->SetInput(smoothing->GetOutput());
	connectedThreshold->SetLower(lowerThreshold);//Valeur en input
	connectedThreshold->SetUpper(upperThreshold);//Valeur en input
	connectedThreshold->SetReplaceValue(255);
	connectedThreshold->SetSeed(index);//Valeur en input
	//Ecriture du writer
	using WriterType = itk::ImageFileWriter< InternalImageType>;
 	auto writer = WriterType::New();
	//caster pour convertir en int les trucs a ecrire.
	//using CasterType = itk::CastImageFilter<InternalImageType, WriterType>;
	//CasterType::Pointer caster = CasterType::New();
	//caster->SetInput(connectedThreshold->GetOutput()); 
	//writer->SetInput(caster->GetOutput());
	writer->SetInput(reader->GetOutput());
  	writer->SetFileName(outputImageFile);
	  
	try
    {
    	writer->Update();
    }
  	catch( itk::ExceptionObject & error )
    {
    	std::cout << "Error: " << error.what() << std::endl;
    }

 	std::cout << "Hello world!" << std::endl;
  return 0;
}