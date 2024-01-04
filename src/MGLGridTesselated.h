#include "MGLGrid.h"

namespace Aftr
{

   class MGLGridTessellated : public MGLGrid
   {
   public:
      static MGLGridTessellated* New(WO* parentWO, const std::vector< std::vector< VectorD > >& points,
         const std::vector< std::vector< aftrColor4ub > >& colors = std::vector< std::vector< aftrColor4ub > >());
      ~MGLGridTessellated() {};
      virtual void onCreate();
     // virtual void defineVertices();

      GLuint vertexBuffer;
      GLuint vaoHandle = 0;
      GLuint numPatches = 0;
   protected:
      MGLGridTessellated(WO* parentWO, const std::vector< std::vector< VectorD > >& points,
         const std::vector< std::vector< aftrColor4ub > >& colors);
      
      virtual void render(const Camera& cam);
     
   };

};