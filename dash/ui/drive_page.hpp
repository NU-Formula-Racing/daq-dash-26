#ifndef __DRIVE_PAGE_H__
#define __DRIVE_PAGE_H__

#include "page.hpp"

#include <okay/okay.hpp>

#include <vector>

namespace dash {

class DrivePage : public IPage {
   public:
    DrivePage() {}

    void initializePage();
    void closePage();

   private:
    okay::UIElement buildSpeedometer();
    okay::UIElement buildTemperatureDisplay();

    std::vector<okay::ECSEntity> _entities;

    okay::Mesh centerMesh{okay::Mesh::none()};
    okay::GameAssetRef<okay::MeshData> centerMeshData{"models/northwestern_n.obj"};
    okay::MaterialHandle objectMaterial;

    // speedometer
    okay::MaterialHandle speedometerMaterial;
    okay::GameAssetRef<okay::Shader> speedometerShader{"shaders/speedometer"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> speedometerTexture{
        "textures/speedometer.png"};
};

}  // namespace dash

#endif
