// Created on: 1994-06-17
// Created by: Modeling
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepFilletAPI_MakeFillet.hxx>
#include <ChFiDS_ErrorStatus.hxx>
#include <ChFiDS_Spine.hxx>
#include <Geom_Surface.hxx>
#include <Law_Linear.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

//=================================================================================================

BRepFilletAPI_MakeFillet::BRepFilletAPI_MakeFillet(const TopoDS_Shape&      S,
                                                   const ChFi3d_FilletShape FShape)
    : myBuilder(S, FShape)
{
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetParams(const Standard_Real Tang,
                                         const Standard_Real Tesp,
                                         const Standard_Real T2d,
                                         const Standard_Real TApp3d,
                                         const Standard_Real TolApp2d,
                                         const Standard_Real Fleche)
{
  myBuilder.SetParams(Tang, Tesp, T2d, TApp3d, TolApp2d, Fleche);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetContinuity(const GeomAbs_Shape InternalContinuity,
                                             const Standard_Real AngleTol)
{
  myBuilder.SetContinuity(InternalContinuity, AngleTol);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const TopoDS_Edge& E)
{
  myBuilder.Add(E);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const Standard_Real Radius, const TopoDS_Edge& E)
{
  // myBuilder.Add(Radius,E);
  myBuilder.Add(E);
  Standard_Integer IinC;
  Standard_Integer IC = myBuilder.Contains(E, IinC);
  if (IC)
    SetRadius(Radius, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const Standard_Real R1,
                                   const Standard_Real R2,
                                   const TopoDS_Edge&  E)
{
  myBuilder.Add(E);
  Standard_Integer IinC;
  Standard_Integer IC = myBuilder.Contains(E, IinC);
  if (IC)
    SetRadius(R1, R2, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const Handle(Law_Function)& L, const TopoDS_Edge& E)
{
  // myBuilder.Add(L,E);
  myBuilder.Add(E);
  Standard_Integer IinC;
  Standard_Integer IC = myBuilder.Contains(E, IinC);
  if (IC)
    SetRadius(L, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const TColgp_Array1OfPnt2d& UandR, const TopoDS_Edge& E)
{
  myBuilder.Add(E);
  Standard_Integer IinC;
  Standard_Integer IC = myBuilder.Contains(E, IinC);
  if (IC)
    SetRadius(UandR, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const Standard_Real    Radius,
                                         const Standard_Integer IC,
                                         const Standard_Integer IinC)
{
  gp_XY FirstUandR(0., Radius), LastUandR(1., Radius);
  myBuilder.SetRadius(FirstUandR, IC, IinC);
  myBuilder.SetRadius(LastUandR, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const Standard_Real    R1,
                                         const Standard_Real    R2,
                                         const Standard_Integer IC,
                                         const Standard_Integer IinC)
{
  Standard_Real r1, r2;

  if (Abs(R1 - R2) < Precision::Confusion())
    r1 = r2 = (R1 + R2) * 0.5;
  else
  {
    r1 = R1;
    r2 = R2;
  }
  gp_XY FirstUandR(0., r1), LastUandR(1., r2);
  myBuilder.SetRadius(FirstUandR, IC, IinC);
  myBuilder.SetRadius(LastUandR, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const Handle(Law_Function)& L,
                                         const Standard_Integer      IC,
                                         const Standard_Integer      IinC)
{
  myBuilder.SetRadius(L, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const TColgp_Array1OfPnt2d& UandR,
                                         const Standard_Integer      IC,
                                         const Standard_Integer      IinC)
{
  if (UandR.Length() == 1)
    SetRadius(UandR(UandR.Lower()).Y(), IC, IinC);
  else if (UandR.Length() == 2)
    SetRadius(UandR(UandR.Lower()).Y(), UandR(UandR.Upper()).Y(), IC, IinC);
  else
  {
    Standard_Real Uf = UandR(UandR.Lower()).X();
    Standard_Real Ul = UandR(UandR.Upper()).X();
    for (Standard_Integer i = UandR.Lower(); i <= UandR.Upper(); i++)
    {
      Standard_Real Ucur = UandR(i).X();
      Ucur               = (Ucur - Uf) / (Ul - Uf);
      gp_XY newUandR(Ucur, UandR(i).Y());
      myBuilder.SetRadius(newUandR, IC, IinC);
    }
  }
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeFillet::IsConstant(const Standard_Integer IC)
{
  return myBuilder.IsConstant(IC);
}

//=================================================================================================

Standard_Real BRepFilletAPI_MakeFillet::Radius(const Standard_Integer IC)
{
  return myBuilder.Radius(IC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::ResetContour(const Standard_Integer IC)
{
  myBuilder.ResetContour(IC);
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeFillet::IsConstant(const Standard_Integer IC,
                                                      const TopoDS_Edge&     E)
{
  return myBuilder.IsConstant(IC, E);
}

//=================================================================================================

Standard_Real BRepFilletAPI_MakeFillet::Radius(const Standard_Integer IC, const TopoDS_Edge& E)
{
  return myBuilder.Radius(IC, E);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const Standard_Real    Radius,
                                         const Standard_Integer IC,
                                         const TopoDS_Edge&     E)
{
  myBuilder.SetRadius(Radius, IC, E);
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeFillet::GetBounds(const Standard_Integer IC,
                                                     const TopoDS_Edge&     E,
                                                     Standard_Real&         F,
                                                     Standard_Real&         L)
{
  return myBuilder.GetBounds(IC, E, F, L);
}

//=================================================================================================

Handle(Law_Function) BRepFilletAPI_MakeFillet::GetLaw(const Standard_Integer IC,
                                                      const TopoDS_Edge&     E)
{
  return myBuilder.GetLaw(IC, E);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetLaw(const Standard_Integer      IC,
                                      const TopoDS_Edge&          E,
                                      const Handle(Law_Function)& L)
{
  myBuilder.SetLaw(IC, E, L);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const Standard_Real    Radius,
                                         const Standard_Integer IC,
                                         const TopoDS_Vertex&   V)
{
  myBuilder.SetRadius(Radius, IC, V);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetFilletShape(const ChFi3d_FilletShape FShape)
{
  myBuilder.SetFilletShape(FShape);
}

//=================================================================================================

ChFi3d_FilletShape BRepFilletAPI_MakeFillet::GetFilletShape() const
{
  return myBuilder.GetFilletShape();
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeFillet::NbContours() const
{
  return myBuilder.NbElements();
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeFillet::Contour(const TopoDS_Edge& E) const
{
  return myBuilder.Contains(E);
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeFillet::NbEdges(const Standard_Integer I) const
{
  const Handle(ChFiDS_Spine)& Spine = myBuilder.Value(I);
  Standard_Integer            n     = Spine->NbEdges();
  return n;
}

//=================================================================================================

const TopoDS_Edge& BRepFilletAPI_MakeFillet::Edge(const Standard_Integer I,
                                                  const Standard_Integer J) const
{
  const Handle(ChFiDS_Spine)& Spine = myBuilder.Value(I);
  const TopoDS_Edge&          S     = Spine->Edges(J);
  return S;
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Remove(const TopoDS_Edge& E)
{
  myBuilder.Remove(E);
}

//=================================================================================================

Standard_Real BRepFilletAPI_MakeFillet::Length(const Standard_Integer IC) const
{
  return myBuilder.Length(IC);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet::FirstVertex(const Standard_Integer IC) const
{
  return myBuilder.FirstVertex(IC);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet::LastVertex(const Standard_Integer IC) const
{
  return myBuilder.LastVertex(IC);
}

//=================================================================================================

Standard_Real BRepFilletAPI_MakeFillet::Abscissa(const Standard_Integer IC,
                                                 const TopoDS_Vertex&   V) const
{
  return myBuilder.Abscissa(IC, V);
}

//=================================================================================================

Standard_Real BRepFilletAPI_MakeFillet::RelativeAbscissa(const Standard_Integer IC,
                                                         const TopoDS_Vertex&   V) const
{
  return myBuilder.RelativeAbscissa(IC, V);
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeFillet::ClosedAndTangent(const Standard_Integer IC) const
{
  return myBuilder.ClosedAndTangent(IC);
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeFillet::Closed(const Standard_Integer IC) const
{
  return myBuilder.Closed(IC);
}

//=================================================================================================

Handle(TopOpeBRepBuild_HBuilder) BRepFilletAPI_MakeFillet::Builder() const
{
  return myBuilder.Builder();
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Build(const Message_ProgressRange& /*theRange*/)
{
  myBuilder.Compute();
  if (myBuilder.IsDone())
  {
    Done();
    myShape = myBuilder.Shape();

    // creation of the Map.
    TopExp_Explorer ex;
    for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next())
    {
      myMap.Add(ex.Current());
    }
  }
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Reset()
{
  NotDone();
  myBuilder.Reset();
  myMap.Clear();
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeFillet::NbSurfaces() const
{
  return (myBuilder.Builder()->DataStructure())->NbSurfaces();
}

//=================================================================================================

const TopTools_ListOfShape& BRepFilletAPI_MakeFillet::NewFaces(const Standard_Integer I)
{
  return (*(TopTools_ListOfShape*)&(myBuilder.Builder()->NewFaces(I)));
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Simulate(const Standard_Integer IC)
{
  myBuilder.Simulate(IC);
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeFillet::NbSurf(const Standard_Integer IC) const
{
  return myBuilder.NbSurf(IC);
}

//=================================================================================================

Handle(ChFiDS_SecHArray1) BRepFilletAPI_MakeFillet::Sect(const Standard_Integer IC,
                                                         const Standard_Integer IS) const
{
  return myBuilder.Sect(IC, IS);
}

//=================================================================================================

const TopTools_ListOfShape& BRepFilletAPI_MakeFillet::Generated(const TopoDS_Shape& EorV)
{
  return myBuilder.Generated(EorV);
}

//=================================================================================================

const TopTools_ListOfShape& BRepFilletAPI_MakeFillet::Modified(const TopoDS_Shape& F)
{
  myGenerated.Clear();

  if (myBuilder.Builder()->IsSplit(F, TopAbs_OUT))
  {
    TopTools_ListIteratorOfListOfShape It(myBuilder.Builder()->Splits(F, TopAbs_OUT));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  if (myBuilder.Builder()->IsSplit(F, TopAbs_IN))
  {
    TopTools_ListIteratorOfListOfShape It(myBuilder.Builder()->Splits(F, TopAbs_IN));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  if (myBuilder.Builder()->IsSplit(F, TopAbs_ON))
  {
    TopTools_ListIteratorOfListOfShape It(myBuilder.Builder()->Splits(F, TopAbs_ON));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  return myGenerated;
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeFillet::IsDeleted(const TopoDS_Shape& F)
{
  if (myMap.Contains(F) || myBuilder.Builder()->IsSplit(F, TopAbs_OUT)
      || myBuilder.Builder()->IsSplit(F, TopAbs_IN) || myBuilder.Builder()->IsSplit(F, TopAbs_ON))
    return Standard_False;

  return Standard_True;
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeFillet::NbFaultyContours() const
{
  return myBuilder.NbFaultyContours();
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeFillet::FaultyContour(const Standard_Integer I) const
{
  return myBuilder.FaultyContour(I);
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeFillet::NbComputedSurfaces(const Standard_Integer IC) const
{
  return myBuilder.NbComputedSurfaces(IC);
}

//=================================================================================================

Handle(Geom_Surface) BRepFilletAPI_MakeFillet::ComputedSurface(const Standard_Integer IC,
                                                               const Standard_Integer IS) const
{
  return myBuilder.ComputedSurface(IC, IS);
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeFillet::NbFaultyVertices() const
{
  return myBuilder.NbFaultyVertices();
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet::FaultyVertex(const Standard_Integer IV) const
{
  return myBuilder.FaultyVertex(IV);
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeFillet::HasResult() const
{
  return myBuilder.HasResult();
}

//=================================================================================================

TopoDS_Shape BRepFilletAPI_MakeFillet::BadShape() const
{
  return myBuilder.BadShape();
}

//=================================================================================================

ChFiDS_ErrorStatus BRepFilletAPI_MakeFillet::StripeStatus(const Standard_Integer IC) const
{
  return myBuilder.StripeStatus(IC);
}
