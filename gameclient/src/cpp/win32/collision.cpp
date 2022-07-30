
bool checkForCollision(Point clickedPoint, SDL_Rect* rect) {
  bool collided = false;

  if (clickedPoint.x > rect->x && clickedPoint.x < (rect->x + rect->w)
      && clickedPoint.y > rect->y && clickedPoint.y < (rect->y + rect->h)) {
    collided = true;
  }

  return collided;
}
