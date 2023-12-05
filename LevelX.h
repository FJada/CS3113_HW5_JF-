#include "Scene.h"
#include "Utility.h"
class LevelX : public Scene {
public:
    int ENEMY_COUNT = 0;
    
    ~LevelX();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
