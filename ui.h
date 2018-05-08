
// Prevent this header file from being included multiple times
#pragma once

// VTK header files
#include <vtkPNGReader.h>
#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkActor.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageData.h>
#include <vtkRenderWindowInteractor.h>

// ITK header files
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkScalarImageKmeansImageFilter.h>

// Qt header files
#include <QFileDialog.h>
#include <QMainWindow.h>
#include <QLayout.h>
#include <QPushButton.h>
#include <QVTKWidget.h>
#include <QSlider.h>

// OpenCV header files
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Class that represents the main window for our application
class ui : public QMainWindow
{
	Q_OBJECT
public:
	// CLASS VARIABLES HERE
	QPushButton *button1;
	QPushButton *button2;
	QPushButton *button3;
	QVTKWidget *viewport1;
	QVTKWidget *viewport2;
	QVTKWidget *viewport3;
	QSlider *slider;
	vtkSmartPointer<vtkPNGReader> reader;
	vtkSmartPointer<vtkPNGReader> reader2;
	vtkSmartPointer<vtkPNGReader> reader3;
	vtkSmartPointer<vtkImageViewer2> viewer1;
	vtkSmartPointer<vtkImageViewer2> viewer2;
	vtkSmartPointer<vtkImageViewer2> viewer3;
	QString file1;


	itk::ImageFileReader< itk::Image< unsigned char, 2 > >::Pointer itkreader;
	itk::ImageFileWriter<itk::Image<unsigned char, 2>>::Pointer writer;
	itk::ScalarImageKmeansImageFilter< itk::Image< unsigned char, 2 > >::Pointer kmeans;

	// Constructor (happens when created)
	ui()
	{
		// Resize the window
		this->resize(800, 400); // CHANGE THIS

								 // Create the "central" (primary) widget for the window
		QWidget *widget = new QWidget();
		this->setCentralWidget(widget);

		// Create your widgets
		// YOUR CODE HERE
		button1 = new QPushButton("Load Cherenkov Dataset");
		button1->setFixedSize(150, 30);
		button2 = new QPushButton("Load RedflectRS Dataset");
		button2->setFixedSize(300, 30);
		viewport1 = new QVTKWidget();
		viewport1->setFixedSize(400, 300);
		viewport2 = new QVTKWidget();
		viewport2->setFixedSize(400, 300);
		viewport3 = new QVTKWidget();
		viewport3->setFixedSize(800, 300);

		slider = new QSlider();
		slider->setOrientation(Qt::Horizontal);


		// Layout the widgets
		// YOUR CODE HERE
		QVBoxLayout *layout_vertical = new QVBoxLayout();
		QVBoxLayout *layout_vertical2 = new QVBoxLayout();
		QVBoxLayout *layout_vertical3 = new QVBoxLayout();
		QHBoxLayout *layout_horizontal1 = new QHBoxLayout();
		QHBoxLayout *layout_horizontal2 = new QHBoxLayout();
		QHBoxLayout *layout_horizontal3 = new QHBoxLayout();
		widget->setLayout(layout_vertical);

		layout_vertical->addLayout(layout_horizontal1);
		layout_horizontal1->addWidget(viewport1);
		layout_vertical->addLayout(layout_horizontal2);
		layout_horizontal2->addWidget(button1);
		layout_horizontal2->addWidget(slider);
		layout_horizontal2->addWidget(button2);

		layout_horizontal1->addWidget(viewport2);


		layout_vertical->addLayout(layout_horizontal3);
		layout_horizontal3->addWidget(viewport3);


		// Connected widget signals to slots
		// YOUR CODE HERE
		connect(button1, SIGNAL(released()), this, SLOT(load_ckov()));
		connect(button2, SIGNAL(released()), this, SLOT(segment()));
		connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slider_changed(int)));

		// Display the window
		this->show();
	}

	public slots: // This tells Qt we are defining slots here

				  // YOUR CODE HERE
	void load_ckov()
	{
		file1 = QFileDialog::getOpenFileName(this,
			tr("Load Cherenkov Data"), "C:/engg199-03", tr("Image Files (*.png)"));

		std::string fileName = file1.toUtf8().constData();
		const char *fileName_final = fileName.c_str();

		//read png file from path
		reader = vtkSmartPointer<vtkPNGReader>::New();
		reader->SetFileName(fileName_final);
		reader->Update();

		// Now implement the image viewer
		viewer1 = vtkSmartPointer<vtkImageViewer2>::New();
		viewer1->SetInputConnection(reader->GetOutputPort());
		viewer1->SetRenderWindow(viewport1->GetRenderWindow());
		viewer1->Render();

	}

	void segment()
	{
		std::string fileName = file1.toUtf8().constData();
		const char *fileName_final = fileName.c_str();

		itkreader = itk::ImageFileReader<itk::Image< unsigned char, 2 >>::New();
		itkreader->SetFileName(fileName_final);

		kmeans = itk::ScalarImageKmeansImageFilter<itk::Image< unsigned char, 2 > >::New();
		kmeans->SetInput(itkreader->GetOutput()); //connects reader to the filter.
												  //specify scalar vaues used initlaly to group pixels so initially we have three intensity levels.
		kmeans->AddClassWithInitialMean(5);
		kmeans->AddClassWithInitialMean(100);
		kmeans->AddClassWithInitialMean(200);
		kmeans->Update(); //which forces an update.

						  //now save the image to a file.
		writer = itk::ImageFileWriter<itk::Image<unsigned char, 2>>::New();
		writer->SetFileName("C:/engg199-03/assignment3/data/segmented_image.png");
		writer->SetInput(kmeans->GetOutput()); //connects kmean filter
		writer->Update(); //saves segmented image file.

						  //read png file from path
		reader2 = vtkSmartPointer<vtkPNGReader>::New();
		reader2->SetFileName("C:/engg199-03/assignment3/data/segmented_image.png");
		reader2->Update();

		// Now implement the image viewer
		viewer2 = vtkSmartPointer<vtkImageViewer2>::New();
		viewer2->SetInputConnection(reader2->GetOutputPort());
		viewer2->SetRenderWindow(viewport2->GetRenderWindow());

		viewer2->GetWindowLevel()->SetLevel(0); // Set the level (middle of display range)
		viewer2->GetWindowLevel()->SetWindow(2); // Set the window (display range)

												 //viewer2->GetWindowLevel()->SetWindow(slider); // Set the window (display range)
		viewer2->Render();

	}

	void slider_changed(int value)
	{
		// set slider min and max.
		slider->setRange(0, 2);

		if (viewer2 == NULL)
			return;

		viewer2->GetWindowLevel()->SetLevel(value); // Set the level 
		viewer2->GetWindowLevel()->SetWindow(2); // Set the window (display range)
		viewer2->Render();
	}


};