//  This is a generated file, DO NOT EDIT!!

#if !defined(Datatypes_Dispatch2_h)
#define Datatypes_Dispatch2_h

// dispatch_scalar2 macro follows
#define dispatch_scalar2(field1, field2, callback)\
  bool disp_error = false;\
  string disp_msg;\
  string disp_name = field2->get_type_name(0);\
  if (disp_name == "TetVol") {\
    if (field2->get_type_name(1) == "double") {\
      TetVol<double> *f2 = 0;\
      f2 = dynamic_cast<TetVol<double>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "int") {\
      TetVol<int> *f2 = 0;\
      f2 = dynamic_cast<TetVol<int>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "short") {\
      TetVol<short> *f2 = 0;\
      f2 = dynamic_cast<TetVol<short>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "unsigned char") {\
      TetVol<unsigned char> *f2 = 0;\
      f2 = dynamic_cast<TetVol<unsigned char>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "bool") {\
      TetVol<bool> *f2 = 0;\
      f2 = dynamic_cast<TetVol<bool>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
      }\
    }\
  } else if (disp_name == "LatticeVol") {\
    if (field2->get_type_name(1) == "double") {\
      LatticeVol<double> *f2 = 0;\
      f2 = dynamic_cast<LatticeVol<double>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "int") {\
      LatticeVol<int> *f2 = 0;\
      f2 = dynamic_cast<LatticeVol<int>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "short") {\
      LatticeVol<short> *f2 = 0;\
      f2 = dynamic_cast<LatticeVol<short>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "unsigned char") {\
      LatticeVol<unsigned char> *f2 = 0;\
      f2 = dynamic_cast<LatticeVol<unsigned char>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "bool") {\
      LatticeVol<bool> *f2 = 0;\
      f2 = dynamic_cast<LatticeVol<bool>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
      }\
    }\
  } else if (disp_name == "ContourField") {\
    if (field2->get_type_name(1) == "double") {\
      ContourField<double> *f2 = 0;\
      f2 = dynamic_cast<ContourField<double>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "int") {\
      ContourField<int> *f2 = 0;\
      f2 = dynamic_cast<ContourField<int>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "short") {\
      ContourField<short> *f2 = 0;\
      f2 = dynamic_cast<ContourField<short>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "unsigned char") {\
      ContourField<unsigned char> *f2 = 0;\
      f2 = dynamic_cast<ContourField<unsigned char>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "bool") {\
      ContourField<bool> *f2 = 0;\
      f2 = dynamic_cast<ContourField<bool>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
      }\
    }\
  } else if (disp_name == "TriSurf") {\
    if (field2->get_type_name(1) == "double") {\
      TriSurf<double> *f2 = 0;\
      f2 = dynamic_cast<TriSurf<double>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "int") {\
      TriSurf<int> *f2 = 0;\
      f2 = dynamic_cast<TriSurf<int>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "short") {\
      TriSurf<short> *f2 = 0;\
      f2 = dynamic_cast<TriSurf<short>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "unsigned char") {\
      TriSurf<unsigned char> *f2 = 0;\
      f2 = dynamic_cast<TriSurf<unsigned char>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "bool") {\
      TriSurf<bool> *f2 = 0;\
      f2 = dynamic_cast<TriSurf<bool>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
      }\
    }\
  } else if (disp_name == "PointCloud") {\
    if (field2->get_type_name(1) == "double") {\
      PointCloud<double> *f2 = 0;\
      f2 = dynamic_cast<PointCloud<double>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "int") {\
      PointCloud<int> *f2 = 0;\
      f2 = dynamic_cast<PointCloud<int>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "short") {\
      PointCloud<short> *f2 = 0;\
      f2 = dynamic_cast<PointCloud<short>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "unsigned char") {\
      PointCloud<unsigned char> *f2 = 0;\
      f2 = dynamic_cast<PointCloud<unsigned char>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
      }\
    } else if (field2->get_type_name(1) == "bool") {\
      PointCloud<bool> *f2 = 0;\
      f2 = dynamic_cast<PointCloud<bool>*>(field2.get_rep());\
      if (f2) {\
        if (disp_name == "TetVol") {\
          if (field1->get_type_name(1) == "double") {\
            TetVol<double> *f1 = 0;\
            f1 = dynamic_cast<TetVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TetVol<int> *f1 = 0;\
            f1 = dynamic_cast<TetVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TetVol<short> *f1 = 0;\
            f1 = dynamic_cast<TetVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TetVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TetVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TetVol<bool> *f1 = 0;\
            f1 = dynamic_cast<TetVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TetVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "LatticeVol") {\
          if (field1->get_type_name(1) == "double") {\
            LatticeVol<double> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            LatticeVol<int> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            LatticeVol<short> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            LatticeVol<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            LatticeVol<bool> *f1 = 0;\
            f1 = dynamic_cast<LatticeVol<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "LatticeVol<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "ContourField") {\
          if (field1->get_type_name(1) == "double") {\
            ContourField<double> *f1 = 0;\
            f1 = dynamic_cast<ContourField<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            ContourField<int> *f1 = 0;\
            f1 = dynamic_cast<ContourField<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            ContourField<short> *f1 = 0;\
            f1 = dynamic_cast<ContourField<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            ContourField<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<ContourField<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            ContourField<bool> *f1 = 0;\
            f1 = dynamic_cast<ContourField<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "ContourField<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "TriSurf") {\
          if (field1->get_type_name(1) == "double") {\
            TriSurf<double> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            TriSurf<int> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            TriSurf<short> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            TriSurf<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            TriSurf<bool> *f1 = 0;\
            f1 = dynamic_cast<TriSurf<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "TriSurf<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_name == "PointCloud") {\
          if (field1->get_type_name(1) == "double") {\
            PointCloud<double> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<double>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<double>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "int") {\
            PointCloud<int> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<int>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<int>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "short") {\
            PointCloud<short> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<short>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<short>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "unsigned char") {\
            PointCloud<unsigned char> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<unsigned char>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<unsigned char>::get_type_name is broken";\
            }\
          } else if (field1->get_type_name(1) == "bool") {\
            PointCloud<bool> *f1 = 0;\
            f1 = dynamic_cast<PointCloud<bool>*>(field1.get_rep());\
            if (f1) {\
              callback(f1, f2);\
            } else {\
              disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
            }\
          }\
        } else if (disp_error) {\
          cerr << "Error: " << disp_msg << endl;\
        }\
      } else {\
        disp_error = true; disp_msg = "PointCloud<bool>::get_type_name is broken";\
      }\
    }\
  } else if (disp_error) {\
    cerr << "Error: " << disp_msg << endl;\
  }\

#endif //Datatypes_Dispatch2_h
