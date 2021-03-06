fast_add_sources(
    VTKMeshFileImporter.cpp
    VTKMeshFileImporter.hpp
    MetaImageImporter.cpp
    MetaImageImporter.hpp
    Importer.hpp
    ImageFileImporter.cpp
    ImageFileImporter.hpp
)
fast_add_process_object(ImageFileImporter ImageFileImporter.hpp)
fast_add_process_object(MetaImageImporter MetaImageImporter.hpp)
fast_add_process_object(VTKMeshFileImporter VTKMeshFileImporter.hpp)
fast_add_test_sources(
    Tests/MetaImageImporterTests.cpp
    Tests/VTKMeshFileImporterTests.cpp
    Tests/ImageFileImporterTests.cpp
)
if(FAST_MODULE_Dicom)
fast_add_sources(
    DICOMFileImporter.cpp
    DICOMFileImporter.hpp
)
fast_add_process_object(DICOMFileImporter DICOMFileImporter.hpp)
fast_add_test_sources(
    Tests/DICOMFileImporterTests.cpp
)
endif()
if(FAST_MODULE_Visualization)
    fast_add_sources(
        ImageImporter.cpp
        ImageImporter.hpp
    )
    fast_add_test_sources(
        Tests/ImageImporterTests.cpp
    )
endif()
if(FAST_MODULE_VTK)
    fast_add_sources(
        VTKImageImporter.cpp
        VTKImageImporter.hpp
    )
    fast_add_test_sources(
        Tests/VTKImageImporterTests.cpp
    )
endif()
if(FAST_MODULE_ITK)
    fast_add_sources(
        ITKImageImporter.hpp
    )
    fast_add_test_sources(
        Tests/ITKImageImporterTests.cpp
    )
endif()

if(FAST_MODULE_WholeSlideImaging)
    fast_add_sources(
        WholeSlideImageImporter.cpp
        WholeSlideImageImporter.hpp
    )
    fast_add_test_sources(
        Tests/WholeSlideImageImporterTests.cpp
    )
    fast_add_process_object(WholeSlideImageImporter WholeSlideImageImporter.hpp)
endif()
if(FAST_MODULE_HDF5)
    fast_add_sources(
        HDF5TensorImporter.cpp
        HDF5TensorImporter.hpp
    )
    fast_add_process_object(HDF5TensorImporter HDF5TensorImporter.hpp)
    fast_add_test_sources(Tests/HDF5TensorImporterTests.cpp)
endif()