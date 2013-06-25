#ifndef GEOMETRY_COMPONENTS_H
#define GEOMETRY_COMPONENTS_H

#include <QtCore/QString>
#include <QtCore/QPoint>

class Shape{
  public:
    QString sname;
    QPoint cordii[20],approx;
    int cordi_count;
    Shape();
    void setShape(QString n);
    void setCordinate(double &a, double &b);
    void setApprox(double &a, double &b);
    void display();
    double size();
};

class Key{
public:
  QString name,shapeName;
  double offset;
  QPoint position;
  Key();
  void getKey(double &o);
  void setKeyPosition(double &x,double &y);
  void showKey();
};

class Row{
public:
  double top,left;
  int keyCount;
  QString shapeName;
  Key keyList[30];
  Row();
  void getRow(double t,double l);
  void addKey();
  void displayRow();
};

class Section{
public:
  QString name,shapeName;
  double top,left,angle;
  int rowCount;
  Row rowList[20];
  Section();
  void getName(QString n);
  void addRow();
  void displaySection();
};

class Geometry{
public:
  QString name,description,keyShape;
  int width,height,shape_count;
  double sectionTop,sectionLeft,rowTop,rowLeft,keyGap;
  int sectionCount;
  Shape shapes[40];
  Section sectionList[20];
  Geometry();
  void getName(QString n);
  void getDescription(QString n);
  void getWidth(int a);
  void getHeight(int a);
  void getShapeName(QString n);
  void getShapeCord(double a, double b);
  void getShapeApprox(double a, double b);
  void getShape();
  void display();
  void addSection();
  Shape findShape(QString name);
};



#endif //geometry_componets.h
