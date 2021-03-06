#include "ExamplePythonImageInverter.h"

#include <mitkImageToItk.h>
#include <mitkITKImageImport.h>
#include "mitkLogMacros.h"

#include <QFile>

#include <usGetModuleContext.h>
#include <mitkIPythonService.h>
#include <usModuleContext.h>

namespace captk {

ExamplePythonImageInverter::ExamplePythonImageInverter()
{
	// any setup required goes here
}

void ExamplePythonImageInverter::SetInput(mitk::Image::Pointer inImagePtr)
{
	this->m_InputPtr = inImagePtr;
}

void ExamplePythonImageInverter::Update()
{
	//load python service
	us::ModuleContext* context = us::GetModuleContext();
	us::ServiceReference<mitk::IPythonService> m_PythonServiceRef = context->GetServiceReference<mitk::IPythonService>();
	mitk::IPythonService* m_PythonService = dynamic_cast<mitk::IPythonService*> (context->GetService<mitk::IPythonService>(m_PythonServiceRef));
	mitk::IPythonService::ForceLoadModule();

	// transfer input image to python
	m_PythonService->CopyToPythonAsSimpleItkImage(m_InputPtr, "in_image");

	//read script into QString
	QString data;
	QString pythonScriptName(":/QExamplePython/invertImage.py");
	QFile file(pythonScriptName);
	if (!file.open(QIODevice::ReadOnly))
	{
		MITK_INFO << "filenot opened" << endl;
	}
	else
	{
		MITK_INFO << "file opened" << endl;
		data = file.readAll();
	}
	file.close();

	//call python script
	m_PythonService->Execute(data.toStdString(), mitk::IPythonService::MULTI_LINE_COMMAND);

	// clean up after running script (better way than deleting individual variables?)
	if (m_PythonService->DoesVariableExist("in_image"))
		m_PythonService->Execute("del in_image");

	if (m_PythonService->DoesVariableExist("out_image"))
	{
		//get image back from python
		m_LastOutput = m_PythonService->CopySimpleItkImageFromPython("out_image");
	}

}

mitk::Image::Pointer ExamplePythonImageInverter::GetOutput()
{
	return this->m_LastOutput;
}

} // end namespace captk

