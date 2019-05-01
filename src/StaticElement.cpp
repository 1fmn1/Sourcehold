#include <StaticElement.h>

using namespace Sourcehold::Rendering;

StaticElement::StaticElement(std::shared_ptr<GameManager> mgr, double x, double y, SDL_Rect r) :
    manager(mgr),
    EventConsumer<Mouse>(mgr->GetHandler())
{
    nx = x;
    ny = y;
    rect = r;
}

StaticElement::StaticElement(const StaticElement &elem) :
    manager(elem.manager),
    EventConsumer<Mouse>(elem.manager->GetHandler())
{
    this->rect = elem.rect;
    this->shown = elem.shown;
    this->nx = elem.nx;
    this->ny = elem.ny;
    this->nw = elem.nw;
    this->nh = elem.nh;
}

StaticElement::~StaticElement() {

}

void StaticElement::Hide() {
    shown = false;
}

void StaticElement::Show() {
    shown = true;
}

void StaticElement::Destroy() {

}

void StaticElement::Translate(int x, int y) {
    nx = manager->NormalizeX(x);
    ny = manager->NormalizeY(y);
    tx = manager->NormalizeTargetX(x);
    ty = manager->NormalizeTargetY(y);
}

void StaticElement::Translate(double x, double y) {
    nx = tx = x;
    ny = ty = y;
}

void StaticElement::Scale(int w, int h) {
    nw = manager->NormalizeX(w);
    nh = manager->NormalizeY(h);
    tw = manager->NormalizeTargetX(w);
    th = manager->NormalizeTargetY(h);
}

void StaticElement::Scale(double w, double h) {
    nw = tw = w;
    nh = tw = h;
}

void StaticElement::Render(std::function<Texture&()> render_fn) {
    Texture &elem = render_fn();
    SDL_Rect r = elem.GetRect();
    manager->Render(elem, nx, ny, nw, nh, &r);
}

void StaticElement::Render(Texture &elem, bool whole) {
    SDL_Rect r = elem.GetRect();
    manager->Render(elem, nx, ny, nw, nh, whole ? nullptr : &r);
}

bool StaticElement::IsMouseOver() {
    int rw = manager->ToCoordX(tw) * manager->GetTargetWidth();
    int rh = manager->ToCoordY(th) * manager->GetTargetHeight();
    int rx = manager->ToCoordX(manager->GetTargetX()) + tx * (double)manager->ToCoordX(manager->GetTargetWidth());
    int ry = manager->ToCoordY(manager->GetTargetY()) + ty * (double)manager->ToCoordY(manager->GetTargetHeight());

    if(mouseX > rx && mouseY > ry && mouseX < rx+rw && mouseY < ry+rh) return true;
    return false;
}

bool StaticElement::IsClicked() {
    if(clicked) {
        clicked = false;
        return true;
    } else return false;
}

void StaticElement::onEventReceive(Mouse &event) {
    EventType type = event.GetType();

    if(type == MOUSE_MOTION) {
        mouseX = event.GetPosX();
        mouseY = event.GetPosY();
    }else if(type == MOUSE_BUTTONDOWN) {
        if(IsMouseOver()) clicked = true;
    }
}

