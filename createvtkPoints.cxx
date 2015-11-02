#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkMatrix3x3.h>

#include <iostream>
#include <array>

template<typename coord_t>
vtkSmartPointer<vtkPoints> oldSetPoints()
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
vtkSmartPointer<vtkPoints> newSetPoints()
{
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  
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
  
  coord_t in[2];
  float *begin = static_cast<float*>(points->GetVoidPointer(0));
  
  // Array with the point IDs (only set where needed)
  for (int z = 0; z < nPointsZ; z++) {
    in[1] = (minZ + (static_cast<coord_t>(z) * incrementZ)); // Calculate increment in z;
    for (int y = 0; y < nPointsY; y++) {
      in[0] = (minY + (static_cast<coord_t>(y) * incrementY)); // Calculate increment in y;
      for (int x = 0; x < nPointsX; x++) {
        begin[0] = (minX + (static_cast<coord_t>(x) * incrementX)); // Calculate increment in x;
        begin[1] = in[0];
        begin[2] = in[1];
        begin = begin+3;
      }
    }
  }
  return points;
}

int main(int, char *[])
{
  
  typedef float coord_t;
    
  auto start = std::chrono::high_resolution_clock::now();
  auto oldPoints = oldSetPoints<coord_t>();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout << elapsed_seconds.count() << std::endl;
  oldPoints->Print(std::cout);
  
  start = std::chrono::high_resolution_clock::now();
  auto oldPoints4 = newSetPoints<coord_t>();
  end = std::chrono::high_resolution_clock::now();
  elapsed_seconds = end-start;
  std::cout << elapsed_seconds.count() << std::endl;
  oldPoints4->Print(std::cout);

  return EXIT_SUCCESS;
}
