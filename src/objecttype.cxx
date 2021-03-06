#include "objecttype.hxx"
#include "butler.hxx"

map<int, ObjectInfo> object_info = {
    { SellObject, { SellObject, "Sell", "nosellobject", 0 } }, // Special :)
    { Elevator, { Elevator, "Elevator", "elevator.png", 10 } },
    { Bed, { Bed, "Bed", "bed.png", 10 } },
};

ObjectInfo *get_info(ObjectType type) {
    auto it = object_info.find(type);
    if (it == object_info.end()) {
        printf("Error! Could not find ObjectType %d!\n", type);
        exit(-1);
    }
    return &it->second;
}

sf::Sprite create_object_sprite(ObjectType type) {
    auto it = object_info.find(type);
    if (it == object_info.end()) {
        printf("Error type %d not found in object_info!\n", type);
        exit(-1);
    }
    return create_sprite(it->second.spr_path);
}

