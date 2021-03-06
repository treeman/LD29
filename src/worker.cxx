#include "worker.hxx"
#include "butler.hxx"
#include "world.hxx"
#include "constants.hxx"

Worker::Worker(int x, int y, World *_world) : pos(x, y), world(_world){
    spr = create_sprite("worker.png");
    tile_pos = world->world2tile(x, y);
    current_task.is_done = true;
    txt = create_txt("arial.ttf", 14);
    has_work_time = false;
    work_time = 0;
}

bool Worker::is_free() {
    return current_task.is_done;
}
bool Worker::assign_task(Task task) {
    //printf("I have a task type: %d\n", task.type);

    if (tile_pos == task.pos) {
        current_task = task;
    }
    else {
        path = world->pathfind(tile_pos, task.pos);
        if (!path.size()) return false;
        current_task = task;
    }
    has_work_time = false;
    return true;
}

void Worker::update(const sf::Time &dt) {
    if (!path.empty())
        follow_path(dt);

    if (path.empty() && !is_free()) {
        // Align!
        tile_pos = current_task.pos;
        pos = sf::Vector2f(world->tile2world(tile_pos));

        // Work on task, it's not instant.
        if (!has_work_time) {
            has_work_time = true;

            work_time = 0;

            if (current_task.type == BuildRoom) {
                // Need to remove current tile
                RoomType curr_type = world->get_tile_type(tile_pos.x, tile_pos.y);
                work_time += get_info(curr_type)->remove_time;
                // Time to build current tile
                work_time += get_info(current_task.room_type)->build_time;
            }
            else if (current_task.type == PlaceObject) {
                // TODO
            }

            work_clock.restart();
            progressbar.set_target_time(work_time);
            progressbar.reset();
        }

        float work_done = work_clock.getElapsedTime().asSeconds();
        progressbar.set_completion(work_done);

        if (work_done >= work_time) {
            world->task_done(current_task);
            current_task.is_done = true;
            has_work_time = false;
        }
    }

    progressbar.set_position(pos.x, pos.y - 11);
}
void Worker::draw(sf::RenderWindow &w) {
    spr.setPosition(pos.x, pos.y);
    w.draw(spr);
    if (has_work_time && !is_free())
        progressbar.draw(w);

    // Debug things ^^
#if 0
    // Draw path
    sf::VertexArray lines(sf::LinesStrip);
    for (auto next : path) {
        // Draw at center
        auto p = world->tile2world(next);
        auto pos = sf::Vector2f(p.x + tile_width / 2, p.y + tile_width / 2);
        lines.append(sf::Vertex(pos, make_color(0x42F207)));
    }
    w.draw(lines);

    // Draw information
    int x = pos.x + 25, y = pos.y - 10, dy = 16;
    stringstream ss;
    ss << "pos: " << pos;
    txt.setString(ss.str());
    txt.setPosition(x, y);
    w.draw(txt);

    y += dy;
    ss.str("");
    ss << "tile" << tile_pos;
    txt.setString(ss.str());
    txt.setPosition(x, y);
    w.draw(txt);
#endif
}

void Worker::follow_path(const sf::Time &dt) {
    // Check for finish
    sf::Vector2f a(pos), b(world->tile2world(path.back()));
    if (len(b - a) < 0.05) {
        path.pop_back();
        //printf("now path of length: %d\n", (int)path.size());
        //for (auto p : path) printf(" %d,%d\n", p.x, p.y);
    }
    if (path.empty()) return;

    sf::Vector2f from(pos), to(world->tile2world(path.back()));
    auto norm = normalize((to - from));
    auto diff = norm * dt.asSeconds() * 100.0f;
    pos = pos + diff;
    tile_pos = world->world2tile(sf::Vector2i(pos));
}

WorkerPtr create_worker(int x, int y, World *world) {
    return WorkerPtr(new Worker(x, y, world));
}

