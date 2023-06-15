#ifndef ARKANOID_ELEMENTS_CPP
#define ARKANOID_ELEMENTS_CPP

#include "arkanoid.pb.h"
#include "box2d-incl/box2d/b2_circle_shape.h"
#include "box2d-incl/box2d/b2_fixture.h"
#include "box2d-incl/box2d/b2_math.h"
#include "box2d-incl/box2d/b2_polygon_shape.h"
#include "box2d-incl/box2d/b2_world.h"
#include <cmath>
#include <cstring>
#include <ftxui/screen/color.hpp>
#include <iostream>

#include "utils.hpp"

namespace arkanoid {

int constexpr canvas_width{ 167 }, canvas_height{ 200 };
int constexpr playing_field_top{ 5 }, playing_field_bottom{ canvas_height - 30 }, playing_field_left{ 1 },
  playing_field_right{ canvas_width - 2 };
int constexpr ball_radius{ 1 }, ball_speed{ 1 };
int constexpr paddle_width{ 14 }, paddle_height{ 1 };
int constexpr brick_width{ 14 }, brick_height{ 5 };
int constexpr num_bricks_y{ 4 };
int constexpr brick_distance_x{ 2 }, brick_distance_y{ 3 };
float constexpr b2_coord_convertion_rate{ 140.0F };
int constexpr brick_max_duration{ 3 }, brick_min_duration{ 1 };
float constexpr ball_velocity_x{ 2.0F }, ball_velocity_y{ 2.5F };
float constexpr ball_force_y{ 0.05F }, ball_force_push_when_zero{ 0.7F };
bool constexpr ball_forces{ true };

class IdGenerator
{
private:
  int m_id;

public:
  explicit IdGenerator(int const start_with) : m_id{ start_with - 1 } {}
  [[nodiscard]] int next() { return ++m_id; }

  [[nodiscard]] int current() const { return m_id; }
};

struct Vector
{
public:
  float x, y;

  Vector(int const x, int const y) : x(x), y(y) {}
  Vector(float const x, float const y) : x{ x }, y{ y } {}// todo explicit

  [[nodiscard]] Vector invert() const { return { -x, -y }; }

  [[nodiscard]] Vector add(float const x, float const y) const { return { this->x + x, this->y + y }; }
  [[nodiscard]] Vector add(Vector const &vec) const { return add(vec.x, vec.y); }


  [[nodiscard]] Vector sub(float const x, float const y) const { return { this->x - x, this->y - y }; }
  [[nodiscard]] Vector sub(Vector const &vec) const { return sub(vec.x, vec.y); }

  [[nodistcard]] Vector set_abs(float const abs) const
  {
    Vector const norm = normalize();

    return { norm.x * abs, norm.y * abs };
  }

  [[nodiscard]] Vector normalize() const
  {
    float const a = abs();

    return { x / a, y / a };
  }

  [[nodiscard]] float abs() const { return std::sqrt((x * x) + (y * y)); }

  [[nodiscard]] int x_i() const { return std::round(x); }
  [[nodiscard]] int y_i() const { return std::round(y); }
};

[[nodiscard]] Vector convert_to_b2_coords(Vector const vector)
{
  return { vector.x * b2_coord_convertion_rate, vector.y * b2_coord_convertion_rate };
}
[[nodiscard]] Vector convert_to_arkanoid_coords(Vector const vector)
{
  return { vector.x / b2_coord_convertion_rate, vector.y / b2_coord_convertion_rate };
}

[[nodiscard]] Vector convert_to_arkanoid_coords(b2Vec2 const &vector)
{
  return convert_to_arkanoid_coords(arkanoid::Vector{ vector.x, vector.y });
}


enum ElementType { BALL, BRICK, PADDLE };
class Element
{
public:
  [[nodiscard]] virtual ElementType get_type() const = 0;
  virtual bool did_update() = 0;
  virtual void set_position(Vector const) = 0;
  [[nodiscard]] virtual Vector center_position() const = 0;
  [[nodiscard]] virtual int width() const = 0;
  [[nodiscard]] virtual int height() const = 0;
  [[nodiscard]] virtual bool exists() { return true; }
  [[nodiscard]] virtual ftxui::Color color() const { return m_color; };

protected:
  int m_id;
  ftxui::Color m_color;

  friend void fill_game_element(GameElement *const &, arkanoid::Element const *);// todo: außerhalb von namespace ??
  friend void parse_game_update(std::map<int, std::unique_ptr<Element>> &,
    GameUpdate const &,
    b2World *,
    std::map<b2Fixture *, arkanoid::Element *> &);
  friend void parse_game_element(Element *, GameElement const &);

public:
  IdGenerator static id_generator;

  explicit Element(ftxui::Color const color = ftxui::Color::White) : m_color{ color }, m_id{ id_generator.next() } {}

  void invert_position()
  {
    Vector const position = center_position();
    set_position(
      { playing_field_right + playing_field_left - position.x, playing_field_bottom + playing_field_top - position.y });
  }

  [[nodiscard]] int left() const { return std::round(center_position().x - (width() / 2.0f)); }

  [[nodiscard]] int right() const { return left() + width(); }

  [[nodiscard]] int top() const { return std::round(center_position().y - (height() / 2.0f)); }

  [[nodiscard]] int bottom() const { return top() + height(); }

  [[nodiscard]] int id() const { return m_id; }
};

IdGenerator Element::id_generator = IdGenerator{ 0 };


class Paddle : public Element
{
private:
  b2Body *m_body_ptr = nullptr;
  bool m_updated{ false };
  bool m_is_controlled_by_this_game_instance{ false };
  int m_score{ 0 };
  friend void parse_game_element(Element *, GameElement const &);

public:
  explicit Paddle(Vector const pos, b2World *arkanoid_world, std::map<b2Fixture *, Element *> &map) : Element{}
  {
    auto const position = convert_to_b2_coords(pos);
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(position.x + (width() / 2.0f), position.y + (height() / 2.0f));

    m_body_ptr = arkanoid_world->CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(width() * b2_coord_convertion_rate, height() * b2_coord_convertion_rate);

    auto fixture = m_body_ptr->CreateFixture(&groundBox, 0.0f);

    map.insert({ fixture, this });
  }
  [[nodiscard]] ElementType get_type() const override { return PADDLE; }

  [[nodiscard]] bool update_x(int const new_x)
  {
    Vector old_position = center_position();
    Vector pos = old_position;

    pos.x = new_x;
    set_position(pos);
    if (old_position.x < new_x + 1.0F && old_position.x > new_x - 1.0F) { return false; }// todo: möglicher feinschliff

    m_updated = true;
    return true;
  }

  bool did_update() override
  {
    if (m_updated) {
      m_updated = false;
      return true;
    }

    return false;
  }


  void set_position(Vector const pos) override
  {
    auto const position = convert_to_b2_coords(pos);
    m_body_ptr->SetTransform({ position.x, position.y }, m_body_ptr->GetAngle());
  }

  void set_is_controlled_by_this_game_instance(bool const c) { m_is_controlled_by_this_game_instance = c; }

  [[nodiscard]] Vector center_position() const override
  {
    auto pos = m_body_ptr->GetPosition();
    return convert_to_arkanoid_coords(arkanoid::Vector{ pos.x, pos.y });
  }

  void add_score(int const score)
  {
    m_score += score;
    m_updated = m_updated || (score != 0 && is_controlled_by_this_game_instance());
  }

  [[nodiscard]] int width() const override { return paddle_width; }
  [[nodiscard]] int height() const override { return paddle_height; }
  [[nodiscard]] bool is_controlled_by_this_game_instance() const { return m_is_controlled_by_this_game_instance; }
  [[nodiscard]] ftxui::Color color() const override
  {
    return (is_controlled_by_this_game_instance()) ? ftxui::Color::White : ftxui::Color::GrayDark;
  }
  [[nodiscard]] int score() const { return m_score; }
};
class Ball : public Element
{

private:
  b2Body *m_body_ptr{ nullptr };
  bool m_updated{ false };
  Paddle *m_paddle_ptr{ nullptr };

  friend void parse_game_element(Element *, GameElement const &);

public:
  explicit Ball(Vector const pos, b2World *arkanoid_world, std::map<b2Fixture *, Element *> &map, Vector const velocity)
    : Element{ ftxui::Color::Red }
  {
    auto const position = convert_to_b2_coords(pos);
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);
    // bodyDef.userData = b2BodyUserData{}; // todo: verlinkung auf dieses objekt.
    m_body_ptr = arkanoid_world->CreateBody(&bodyDef);
    bodyDef.linearDamping = 0.0F;
    bodyDef.angularDamping = 0.0F;

    b2CircleShape dynamicBox;
    dynamicBox.m_radius = 1.0F * b2_coord_convertion_rate;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0F;
    fixtureDef.friction = 0.0F;// todo: andere friction = fehlerhaftes verhalten

    auto fixture = m_body_ptr->CreateFixture(&fixtureDef);
    m_body_ptr->SetLinearVelocity({ velocity.x, velocity.y });
    m_body_ptr->SetAngularVelocity(0);
    m_body_ptr->SetFixedRotation(true);

    map.insert({ fixture, this });
  }

  [[nodiscard]] ElementType get_type() const override { return BALL; }

  [[nodiscard]] Vector velocity() const
  {
    auto vel = m_body_ptr->GetLinearVelocity();
    return { vel.x, vel.y };
  }

  bool did_update() override
  {

    if (ball_forces && m_body_ptr != nullptr) {

      float const middle_y = ((playing_field_bottom - playing_field_top) / 2.0F) + playing_field_top;
      float const middle_x = ((playing_field_right - playing_field_left) / 2.0F) + playing_field_left;
      auto const position = center_position();
      auto const vel = velocity();
      auto const gravity = convert_to_b2_coords(Vector{ 0.0F, ball_force_y });
      float correct_direction_x{ 0.0F };
      float corrent_direction_y{ 0.0F };
      if (std::abs(vel.x) <= 0.1F) {// zu linear
        correct_direction_x = (position.x < middle_x) ? ball_force_push_when_zero : -ball_force_push_when_zero;
      }
      if (std::abs(vel.y) <= 0.1F) {
        corrent_direction_y = (position.y < middle_y) ? ball_force_push_when_zero : -ball_force_push_when_zero;
      }

      if (position.y > middle_y) {
        m_body_ptr->ApplyForceToCenter({ gravity.x + correct_direction_x, gravity.y + corrent_direction_y }, false);
      } else if (position.y < middle_y) {
        m_body_ptr->ApplyForceToCenter({ -gravity.x + correct_direction_x, -gravity.y + corrent_direction_y }, false);
      }
    }


    if (m_updated) {
      m_updated = false;
      return true;
    }
    return false;
  }

  void set_position(Vector const pos) override
  {
    auto const position = convert_to_b2_coords(pos);
    m_body_ptr->SetTransform({ position.x, position.y }, 0);
  }

  void set_velocity(Vector const vector)
  {
    if (m_body_ptr != nullptr) { m_body_ptr->SetLinearVelocity({ vector.x, vector.y }); }
  }

  void set_last_paddle(Paddle *const paddle) { m_paddle_ptr = paddle; }

  void add_to_next_update() { m_updated = true; }

  [[nodiscard]] Vector center_position() const override
  {
    auto pos = m_body_ptr->GetPosition();
    return convert_to_arkanoid_coords(arkanoid::Vector{ pos.x, pos.y });
  }

  [[nodiscard]] int width() const override { return ball_radius; }
  [[nodiscard]] int height() const override { return ball_radius; }
  [[nodiscard]] Paddle *last_paddle() const { return m_paddle_ptr; }
};

class Brick : public Element
{

private:
  b2Body *m_body_ptr{ nullptr };
  int m_duration;
  bool m_updated{ false };

  friend void parse_game_element(Element *, GameElement const &);

public:
  explicit Brick(Vector const pos, b2World *arkanoid_world, std::map<b2Fixture *, Element *> &map, int const duration)
    : Element{}, m_duration{ duration }
  // todo: b2world muss als pointer, da sonst make_unique nicht funktioniert
  {

    auto const position = convert_to_b2_coords(pos.add(width() / 2.0F, height() / 2.0F));
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(position.x, position.y);

    m_body_ptr = arkanoid_world->CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(width() * b2_coord_convertion_rate, height() * b2_coord_convertion_rate);

    auto *fixture = m_body_ptr->CreateFixture(&groundBox, 0.0f);

    map.insert({ fixture, this });
  }

  void hit(Ball *ball)
  {
    if ((ball->last_paddle() != nullptr && ball->last_paddle()->is_controlled_by_this_game_instance())
        || m_duration > 1) {
      --m_duration;

      if (m_duration <= 0) {
        m_updated = true;
        ball->add_to_next_update();
      }
    }
  }


  [[nodiscard]] ElementType get_type() const override { return BRICK; }
  void set_position(Vector const pos) override
  {
    auto const position = convert_to_b2_coords(pos);
    m_body_ptr->SetTransform({ position.x, position.y }, m_body_ptr->GetAngle());
  }

  [[nodiscard]] Vector center_position() const override
  {
    auto pos = m_body_ptr->GetPosition();
    return convert_to_arkanoid_coords(arkanoid::Vector{ pos.x, pos.y });
  }

  [[nodiscard]] int width() const override { return brick_width; }
  [[nodiscard]] int height() const override { return brick_height; }
  [[nodiscard]] bool exists() override
  {
    if (m_body_ptr != nullptr && m_duration <= 0 && m_body_ptr->IsEnabled()) { m_body_ptr->SetEnabled(false); }
    return m_duration > 0;
  }
  [[nodiscard]] bool did_update() override
  {
    if (m_updated) {
      m_updated = false;
      return true;
    }

    return false;
  }
  [[nodiscard]] int duration() const { return m_duration; }
  [[nodiscard]] ftxui::Color color() const override
  {
    auto const duration = static_cast<float>(m_duration);
    float const ratio = duration / brick_max_duration;

    if (ratio >= (4.0F / 6.0F)) {
      return ftxui::Color::Yellow;
    } else if (ratio >= (2.0F / 6.0F)) {
      return ftxui::Color::Green;
    }

    return ftxui::Color::White;
  }
};

void fill_game_element(GameElement *const &game_element, arkanoid::Element const *element)
{
  auto *position = new ElementPosition;

  auto const pos = element->center_position();

  float const x = pos.x;
  float const y = pos.y;

  position->set_x(x);
  position->set_y(y);

  game_element->set_id(element->id());
  game_element->set_allocated_element_position(position);

  if (element->get_type() == BALL) {
    auto const *ball = dynamic_cast<const Ball *>(element);
    auto *net_ball = new NetBall;

    auto vel = ball->velocity();

    net_ball->set_velocity_x(vel.x);
    net_ball->set_velocity_y(vel.y);

    game_element->set_allocated_ball(net_ball);
  } else if (element->get_type() == BRICK) {
    auto const *brick = dynamic_cast<const Brick *>(element);
    auto *net_brick = new NetBrick;

    net_brick->set_duration(brick->duration());

    game_element->set_allocated_brick(net_brick);
  } else if (element->get_type() == PADDLE) {
    auto const *paddle = dynamic_cast<const Paddle *>(element);
    auto *net_paddle = new NetPaddle;

    net_paddle->set_controlled_by_sender(paddle->is_controlled_by_this_game_instance());
    net_paddle->set_score(paddle->score());

    game_element->set_allocated_paddle(net_paddle);
  }
}

void fill_game_update(GameUpdate *update, std::vector<arkanoid::Element *> const &elements)
{

  std::for_each(elements.begin(), elements.end(), [&update](arkanoid::Element const *element_ptr) {
    if (element_ptr == nullptr) { return; }
    GameElement *game_element = update->add_element();

    fill_game_element(game_element, element_ptr);
  });
}

void parse_game_element(Element *element, GameElement const &net_element)
{
  element->set_position({ net_element.element_position().x(), net_element.element_position().y() });
  // element->invert_position();

  if (element->get_type() == BALL && net_element.has_ball()) {
    auto *ball = dynamic_cast<Ball *>(element);
    ball->set_velocity(
      arkanoid::Vector{ net_element.ball().velocity_x(), net_element.ball().velocity_y() } /*.invert()*/);
  } else if (element->get_type() == BRICK && net_element.has_brick()) {
    auto *brick = dynamic_cast<Brick *>(element);
    brick->m_duration = net_element.brick().duration();
  } else if (element->get_type() == PADDLE && net_element.has_paddle()) {
    auto *paddle = dynamic_cast<Paddle *>(element);
    paddle->set_is_controlled_by_this_game_instance(!net_element.paddle().controlled_by_sender());
    paddle->m_score = net_element.paddle().score();
  }
}

void parse_game_update(std::map<int, std::unique_ptr<Element>> &map,
  GameUpdate const &update,
  b2World *world,
  std::map<b2Fixture *, arkanoid::Element *> &b2_map)
{
  for (int i = 0; i < update.element_size(); ++i) {
    auto const net_element = update.element(i);
    Vector position{ net_element.element_position().x(), net_element.element_position().y() };

    bool new_id{ !map.contains(net_element.id()) };

    if (new_id) {
      std::unique_ptr<Element> arkanoid_element_ptr = nullptr;

      if (net_element.has_ball()) {
        arkanoid_element_ptr = std::make_unique<Ball>(
          position, world, b2_map, Vector{ net_element.ball().velocity_x(), net_element.ball().velocity_y() });
      } else if (net_element.has_brick()) {
        arkanoid_element_ptr = std::make_unique<Brick>(position, world, b2_map, net_element.brick().duration());
      } else if (net_element.has_paddle()) {
        arkanoid_element_ptr = std::make_unique<Paddle>(position, world, b2_map);
      }
      arkanoid_element_ptr->m_id = net_element.id();
      parse_game_element(arkanoid_element_ptr.get(), net_element);
      insert_element(map, arkanoid_element_ptr);
    } else {
      parse_game_element(map.find(net_element.id())->second.get(), net_element);
    }
  }
}

}// namespace arkanoid

#endif