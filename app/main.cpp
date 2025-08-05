#include "../include/Game_components/Config.h"
#include "../include/Game_components/Ball.h"
#include "../include/Game_components/Table.h"
#include "../include/Game_components/Cue.h"
#include "../include/Graphic/Renderer.h"

using namespace std;
using namespace Biliard;

int main() {
    Renderer renderer(1000, 500, 60);
    renderer.run();
    return 0;
}