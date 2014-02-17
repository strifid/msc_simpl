/*
 * VtpWriter.cpp
 *
 *  Created on: Sep 2, 2013
 *      Author: trifid
 */

#include "VtpWriter.h"
//#include <vtkVersion.h>
//#include <vtkCellArray.h>
//#include <vtkPoints.h>
/*
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPoints2D.h>
*/

VtpWriter::VtpWriter() {
	// TODO Auto-generated constructor stub

}

VtpWriter::~VtpWriter() {
}
// TODO Auto-generated destructor stub

/*
#include <vtkSmartPointer.h>
#include <vtkRegularPolygonSource.h>
#include <vtkPolyData.h>
#include <vtkPoints2D.h>
#include <vtkGlyph2D.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkCellArray.h>
#include <vtkArrayWriter.h>
#include <vtkDataWriter.h>
#include <vtkPolyLine.h>
#include <vtkPolyDataMapper2D.h>
*/

/*
void VtpWriter::saveDesc(DescArcStorage &ascArcStorage) {
void VtpWriter::saveAsc(AscArcStorage &ascArcStorage) {
}
*/



/*
	typename AscArcStorage::ArcsToSeddleMap &ascArcMap = ascArcStorage.arcsToSeddleMap();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

	for(typename AscArcStorage::ArcsToSeddleMap::iterator it = ascArcMap.begin(); it!= ascArcMap.end(); it++){
		AscArcs &ascArcs = it->second;
		for (size_t i = 0; i < ascArcs.size(); ++i) {
			AscArcPtr arc = ascArcs[i];
			vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
			polyLine->GetPointIds()->SetNumberOfIds(arc->m_arc.size());
			for (size_t j = 0; j < arc->m_arc.size(); ++j) {
				VertexPtr vtx = arc->m_arc[j]->maxVertex();
				vtkIdType id = points->InsertNextPoint(vtx->x, vtx->y, 0);
				std::cout << "inset pt x: " <<  vtx->x << " y: " << vtx->y<< std::endl;
				polyLine->GetPointIds()->SetId(j, id);

			}
			cells->InsertNextCell(polyLine);
		}
	}
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);

	polyData->SetLines(cells);
	vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
	writer->SetFileName("test.vtp");
	writer->SetInput(polyData);
	writer->SetDataModeToAscii();
	writer->Write();
*/


