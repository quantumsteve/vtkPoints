#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkMatrix3x3.h>
#include "vtkFloatArray.h"

#include <iostream>
#include <array>

template<typename coord_t>
vtkSmartPointer<vtkPoints> getOldPoints()
{
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    
  coord_t in[3];
    
  const int nBinsX = 501;
  const int nBinsY = 501;
  const int nBinsZ = 501;

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
vtkSmartPointer<vtkPoints> copyPoints(vtkSmartPointer<vtkPoints> oldPoints)
{

  std::array<double,9> skew = {{2.0,0.0,0.0,0.0,2.0,0.0,0.0,0.0,2.0}};
  vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
  newPoints->Allocate(oldPoints->GetNumberOfPoints());
  for (int i = 0; i < oldPoints->GetNumberOfPoints(); ++i) {
    double outPoint[3];
    oldPoints->GetPoint(i, outPoint);
    vtkMatrix3x3::MultiplyPoint(skew.data(), outPoint, outPoint);
    newPoints->InsertNextPoint(outPoint);
  }
  return newPoints;
}

template<typename coord_t>
void updatePoints(vtkPoints* points)
{
  std::array<double,9> skew = {{2.0,0.0,0.0,0.0,2.0,0.0,0.0,0.0,2.0}};
  int last = points->GetNumberOfPoints();
  for (int i = 0; i < last; ++i) {
    double outPoint[3];
    points->GetPoint(i, outPoint);
    vtkMatrix3x3::MultiplyPoint(skew.data(), outPoint, outPoint);
    points->InsertPoint(i, outPoint);
  }
}

template <class ValueType, class Iterator> void iteratorAccess(Iterator begin, Iterator end)
{
    std::array<float,9> skew = {{2.0,0.0,0.0,0.0,2.0,0.0,0.0,0.0,2.0}};
    for(Iterator i = begin; i<end;std::advance(i,3))
    {
        ValueType v1 = i[0];
        ValueType v2 = i[1];
        ValueType v3 = i[2];
        i[0] = v1*skew[0]  + v2*skew[1]  + v3*skew[2];
        i[1] = v1*skew[3]  + v2*skew[4]  + v3*skew[5];
        i[2] = v1*skew[6]  + v2*skew[7]  + v3*skew[8];
    }
}

int main(int, char *[])
{
  
  typedef float coord_t;
  auto oldPoints = getOldPoints<coord_t>();
  
  oldPoints->Print(std::cout);

  auto start = std::chrono::high_resolution_clock::now();
  auto newPoints = copyPoints<coord_t>(oldPoints);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout << elapsed_seconds.count() << std::endl;
  newPoints->Modified();
  newPoints->Print(std::cout);
   
  start = std::chrono::high_resolution_clock::now();
  updatePoints<coord_t>(oldPoints.GetPointer());
  end = std::chrono::high_resolution_clock::now();
  elapsed_seconds = end-start;
  std::cout << elapsed_seconds.count() << std::endl;
  oldPoints->Modified();
  oldPoints->Print(std::cout);
    
  start = std::chrono::high_resolution_clock::now();
  vtkFloatArray* data = vtkFloatArray::SafeDownCast(oldPoints->GetData());
  float * itbegin = data->GetPointer(0);
  float * itend = itbegin + data->GetNumberOfTuples()*3;
  iteratorAccess<float,float*>(itbegin,itend);
  end = std::chrono::high_resolution_clock::now();
  elapsed_seconds = end-start;
  std::cout << elapsed_seconds.count() << std::endl;
  oldPoints->Modified();
  oldPoints->Print(std::cout);
    
  return EXIT_SUCCESS;
}
