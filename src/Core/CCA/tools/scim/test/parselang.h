
%langC++
%ininterface
namespace Example {

class Shape {
public:
  Shape() {
    nshapes++;
  }
  virtual ~Shape() {
    nshapes--;
  };
  double  x, y;   
  void    move(double dx, double dy);
  virtual double area(void) = 0;
  virtual double perimeter(void) = 0;
  static  int nshapes;

  enum SomeEnum {
    First = 0,
    Second,
    Third,
    Last = 1000
  };
};

class Circle : public Shape {
private:
  double radius;
public:
  Circle(double r) : radius(r) { };
  virtual double area(void);
  virtual double perimeter(void);
};
}; // end of namespace Example
%/ininterface
%/langC++


%outinterface
interface sidlCircle {
  double area();
  double perimeter();
};
 
%/outinterface


%map Circle -> sidlCircle 
 
