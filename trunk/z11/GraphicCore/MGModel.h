#ifndef MG_MODELS_H
#define MG_MODELS_H

#include <vector>
#include "GraphicStructure.h"
#include "../CommonBase/MGStructure.h"
namespace z11
{
    enum ModelType
    {
        ModelTypePoints,
        ModelTypeLines,
        ModelTypeLineLoop,
        ModelTypeLineStrip,
        ModelTypeTriangles,
        ModelTypeTriangleStrip,
        ModelTypeTriangleFan
    };

    class MGModel
    {
    public:
        ~MGModel();
        //2D Model
        static MGModel* createRect(const MGRect &rect);
        static MGModel* createRoundedRect(const MGRect &rect,float round_rate,int segments=70);
        static MGModel* createRoundedRect(const MGRect &rect,float round_rate_x,float round_rate_y,int segments=70);
        static MGModel* createSphere(float r,int m,int n);

        //
        MGModel* clone();
        void scale(float scale);
        void scale(float scale_x,float scale_y,float scale_z);
        void translate(float x,float y,float z);
        void draw();
        MGVertex *getVertics();

        //Property
        MGPROPERTY_READONLY(ModelType,m_type,ModelType);
    private:
        std::vector<MGVertex> m_vertics;
        MGModel();
    };

}

#endif // MGMODEL_H
