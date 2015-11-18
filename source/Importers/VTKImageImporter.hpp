#ifndef VTKIMAGEIMPORTER_HPP_
#define VTKIMAGEIMPORTER_HPP_

#include "ProcessObject.hpp"
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include "Image.hpp"

namespace fast {

class VTKImageImporter : public ProcessObject {
    FAST_OBJECT(VTKImageImporter)
    public:
        void setInput(vtkSmartPointer<vtkImageData> image);
    private:
        vtkSmartPointer<vtkImageData> mInput;

        VTKImageImporter();
        void execute();

};

} // end namespace fast



#endif /* VTKIMAGEIMPORTER_HPP_ */