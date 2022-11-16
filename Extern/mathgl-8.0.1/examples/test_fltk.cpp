// g++ test_fltk.cpp -lmgl-fltk -lmgl
#include <mgl2/fltk.h>
int sample(mglGraph *gr)
{
  gr->Rotate(60,40);  gr->Box();
  return 0;
}
//-----------------------------------------------------
int main(int argc,char **argv)
{
  mglFLTK gr(sample,"MathGL examples");
  return gr.Run();
}
