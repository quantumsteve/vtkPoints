#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkMatrix3x3.h>
#include "vtkFloatArray.h"

#include <iostream>
#include <array>
#include <random>

#include "vtkArrayDispatch.h"
#include "vtkAssume.h"
#include "vtkDataArrayAccessor.h"

template<typename coord_t>
vtkSmartPointer<vtkPoints> getOldPoints()
{
    auto points = vtkSmartPointer<vtkPoints>::New();
    
    coord_t in[3];
    
    const int nBinsX = 100;
    const int nBinsY = 100;
    const int nBinsZ = 100;
    
    const coord_t maxX =  10.0;
    const coord_t minX = -10.0;
    const coord_t maxY =  10.0;
    const coord_t minY = -10.0;
    const coord_t maxZ =  10.0;
    const coord_t minZ = -10.0;
    
    const coord_t incrementX = (maxX - minX) / static_cast<coord_t>(nBinsX);
    const coord_t incrementY = (maxY - minY) / static_cast<coord_t>(nBinsY);
    const coord_t incrementZ = (maxZ - minZ) / static_cast<coord_t>(nBinsZ);
    
    const int nPointsX = nBinsX + 1;
    const int nPointsY = nBinsY + 1;
    const int nPointsZ = nBinsZ + 1;
    
    points->SetNumberOfPoints(static_cast<int64_t>(nPointsX)*static_cast<int64_t>(nPointsY)*static_cast<int64_t>(nPointsZ));
    
    // Array with the point IDs (only set where needed)
    for (int z = 0; z < nPointsZ; z++) {
        in[2] = (minZ + (static_cast<coord_t>(z) * incrementZ)); // Calculate increment in z;
        for (int y = 0; y < nPointsY; y++) {
            in[1] = (minY + (static_cast<coord_t>(y) * incrementY)); // Calculate increment in y;
            for (int x = 0; x < nPointsX; x++) {
                in[0] = (minX + (static_cast<coord_t>(x) * incrementX)); // Calculate increment in x;
                points->InsertNextPoint(in);
            }
        }
    }
    return points;
}


template<typename coord_t>
vtkSmartPointer<vtkPoints> SequentialCopy(vtkPoints *oldPoints)
{
    auto newPoints = vtkSmartPointer<vtkPoints>::New();
    vtkIdType numPts = oldPoints->GetNumberOfPoints();
    newPoints->SetNumberOfPoints(numPts);
    for (int i = 0; i < numPts; ++i) {
        double outPoint[3];
        oldPoints->GetPoint(i, outPoint);
        newPoints->SetPoint(i, outPoint);
    }
    return newPoints;
}

template<typename coord_t>
vtkSmartPointer<vtkPoints> RandomCopy(vtkPoints *oldPoints, const std::vector<vtkIdType> &order)
{
    auto newPoints = vtkSmartPointer<vtkPoints>::New();
    vtkIdType numPts = oldPoints->GetNumberOfPoints();
    newPoints->SetNumberOfPoints(numPts);
    for (int i = 0; i < numPts; ++i) {
        double outPoint[3];
        oldPoints->GetPoint(order[i], outPoint);
        newPoints->SetPoint(i, outPoint);
    }
    return newPoints;
}

template<typename coord_t>
vtkSmartPointer<vtkPoints> FasterSequentialCopy(vtkPoints *oldPoints)
{
    auto newPoints = vtkSmartPointer<vtkPoints>::New();
    vtkIdType numPts = oldPoints->GetNumberOfPoints();
    newPoints->SetNumberOfPoints(numPts);
    newPoints->InsertPoints(0,numPts,0,oldPoints);
    return newPoints;
}

struct FunctionWorker {
  const vtkIdType * Order;
  FunctionWorker(const vtkIdType *order) : Order(order) {}
  template <typename SourceArray, typename DestinationArray>
  void operator()(SourceArray *input, DestinationArray *output)
  {
    VTK_ASSUME(input->GetNumberOfComponents() == 3);
    VTK_ASSUME(output->GetNumberOfComponents() == 3);
    vtkIdType numTuples = input->GetNumberOfTuples();
    vtkDataArrayAccessor<SourceArray> src(input);
    vtkDataArrayAccessor<DestinationArray> dest(output);
    for (vtkIdType i = 0; i < numTuples; ++i)
    {
      vtkIdType inIdx = Order[i];
      dest.Set(i,0,src.Get(inIdx, 0));
      dest.Set(i,1,src.Get(inIdx, 1));
      dest.Set(i,2,src.Get(inIdx, 2));
    }
  }
};

template<typename coord_t>
vtkSmartPointer<vtkPoints> FastestRandomCopy(vtkPoints *oldPoints, const std::vector<vtkIdType> &order)
{
    vtkIdType numPts = oldPoints->GetNumberOfPoints();
    auto newPoints = vtkSmartPointer<vtkPoints>::New();
    newPoints->SetNumberOfPoints(numPts);
    
    typedef vtkTypeList_Create_1(vtkFloatArray) Types;
    typedef vtkArrayDispatch::Dispatch2ByArrayWithSameValueType<Types,Types> MyDispatch;
    
    auto input = oldPoints->GetData();
    auto output = newPoints->GetData();
    FunctionWorker worker(order.data());
    if (!MyDispatch::Execute(input, output, worker))
    {
        worker(input, output); // Use vtkDataArray API if dispatch fails.
    }
    return newPoints;
}


template<typename coord_t>
vtkSmartPointer<vtkPoints> FasterRandomCopy(vtkPoints *oldPoints, const std::vector<vtkIdType> &order)
{
    auto newPoints = vtkSmartPointer<vtkPoints>::New();
    vtkIdType numPts = oldPoints->GetNumberOfPoints();
    newPoints->SetNumberOfPoints(numPts);
    for (int i = 0; i < numPts; ++i) {
        newPoints->GetData()->SetTuple(i,order[i],oldPoints->GetData());
    }
    return newPoints;
}

template<typename coord_t>
vtkSmartPointer<vtkPoints> UnsafeRandomCopy(vtkPoints *oldPoints, const std::vector<vtkIdType> &order)
{
    auto newPoints = vtkSmartPointer<vtkPoints>::New();
    vtkIdType numPts = oldPoints->GetNumberOfPoints();
    newPoints->SetNumberOfPoints(numPts);
    auto oldPtr = static_cast<float *>(oldPoints->GetVoidPointer(0));
    auto newPtr = static_cast<float *>(newPoints->GetVoidPointer(0));
    for (int i = 0; i < numPts; ++i) {
        newPtr[3*i] = oldPtr[3*order[i]];
        newPtr[3*i+1] = oldPtr[3*order[i]+1];
        newPtr[3*i+2] = oldPtr[3*order[i]+2];
    }
    return newPoints;
}

int main(int, char *[])
{
    
    typedef float coord_t;
    auto oldPoints = getOldPoints<coord_t>();
    //oldPoints->Print(std::cout);
    vtkIdType numPts = oldPoints->GetNumberOfPoints();
    
    // Specify the engine and distribution.
    std::mt19937 mersenne_engine;
    std::uniform_int_distribution<vtkIdType> dist(0, numPts - 1);
    auto gen = std::bind(dist, mersenne_engine);
    std::vector<vtkIdType> order(numPts);
    generate(std::begin(order), std::end(order), gen);
    
    auto start = std::chrono::high_resolution_clock::now();
    auto newPoints = SequentialCopy<coord_t>(oldPoints);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "SequentialCopy: " << elapsed_seconds.count() << std::endl;
    //newPoints->Modified();
    //newPoints->Print(std::cout);
    
    start = std::chrono::high_resolution_clock::now();
    newPoints = RandomCopy<coord_t>(oldPoints.GetPointer(),order);
    end = std::chrono::high_resolution_clock::now();
    elapsed_seconds = end-start;
    std::cout << "RandomCopy: " << elapsed_seconds.count() << std::endl;
    //oldPoints->Modified();
    //oldPoints->Print(std::cout);
    
    start = std::chrono::high_resolution_clock::now();
    newPoints = FasterSequentialCopy<coord_t>(oldPoints.GetPointer());
    end = std::chrono::high_resolution_clock::now();
    elapsed_seconds = end-start;
    std::cout << "FasterSequentialCopy: " << elapsed_seconds.count() << std::endl;
    //oldPoints->Modified();
    //oldPoints->Print(std::cout);
    
    start = std::chrono::high_resolution_clock::now();
    newPoints = FasterRandomCopy<coord_t>(oldPoints.GetPointer(),order);
    end = std::chrono::high_resolution_clock::now();
    elapsed_seconds = end-start;
    std::cout << "FasterRandomCopy:" << elapsed_seconds.count() << std::endl;
    //oldPoints->Modified();
    //oldPoints->Print(std::cout);
    
    start = std::chrono::high_resolution_clock::now();
    newPoints = FastestRandomCopy<coord_t>(oldPoints.GetPointer(),order);
    end = std::chrono::high_resolution_clock::now();
    elapsed_seconds = end-start;
    std::cout << "FastestRandomCopy:" << elapsed_seconds.count() << std::endl;
    //oldPoints->Modified();
    //oldPoints->Print(std::cout);
    
    start = std::chrono::high_resolution_clock::now();
    newPoints = UnsafeRandomCopy<coord_t>(oldPoints.GetPointer(),order);
    end = std::chrono::high_resolution_clock::now();
    elapsed_seconds = end-start;
    std::cout << "UnsafeRandomCopy: " << elapsed_seconds.count() << std::endl;
    
    //oldPoints->Modified();
    //oldPoints->Print(std::cout);
    
    return EXIT_SUCCESS;
}
