#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SPHERES 16
#define MAX_MATERIALS 16

#define LAMBERTIAN 0
#define METAL 1

typedef struct {
    double x;
    double y;
    double z;
} Vec3;

typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef struct {
    int type;
    Vec3 color;
    double fuzz;
} Material;

typedef struct {
    Vec3 center;
    double radius;
    int material;
} Sphere;

typedef struct {
    Vec3 point;
    Vec3 normal;
    double t;
    int material;
} Hit;

Material materials[MAX_MATERIALS];
Sphere spheres[MAX_SPHERES];
int material_count = 0;
int sphere_count = 0;

Vec3 vec3(double x, double y, double z) {
    Vec3 v = {x, y, z};
    return v;
}

Vec3 add(Vec3 a, Vec3 b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 subtract(Vec3 a, Vec3 b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 multiply(Vec3 a, Vec3 b) {
    return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vec3 scale(Vec3 v, double n) {
    return vec3(v.x * n, v.y * n, v.z * n);
}

double dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double length_squared(Vec3 v) {
    return dot(v, v);
}

Vec3 unit_vector(Vec3 v) {
    return scale(v, 1.0 / sqrt(length_squared(v)));
}

double random_double(void) {
    return rand() / (RAND_MAX + 1.0);
}

Vec3 random_unit_vector(void) {
    while (1) {
        Vec3 v = vec3(
            2.0 * random_double() - 1.0,
            2.0 * random_double() - 1.0,
            2.0 * random_double() - 1.0
        );
        double length = length_squared(v);

        if (length > 1e-160 && length <= 1.0)
            return scale(v, 1.0 / sqrt(length));
    }
}

Vec3 reflect(Vec3 v, Vec3 normal) {
    return subtract(v, scale(normal, 2.0 * dot(v, normal)));
}

Vec3 ray_at(Ray ray, double t) {
    return add(ray.origin, scale(ray.direction, t));
}

int add_material(int type, Vec3 color, double fuzz) {
    int id = material_count;
    materials[id].type = type;
    materials[id].color = color;
    materials[id].fuzz = fuzz > 1.0 ? 1.0 : fuzz;
    material_count++;
    return id;
}

void add_sphere(Vec3 center, double radius, int material) {
    spheres[sphere_count].center = center;
    spheres[sphere_count].radius = radius;
    spheres[sphere_count].material = material;
    sphere_count++;
}

int hit_sphere(Sphere sphere, Ray ray, double min_t, double max_t, Hit *hit) {
    Vec3 offset = subtract(sphere.center, ray.origin);
    double a = length_squared(ray.direction);
    double h = dot(ray.direction, offset);
    double c = length_squared(offset) - sphere.radius * sphere.radius;
    double discriminant = h * h - a * c;
    double root;
    Vec3 outward_normal;

    if (discriminant < 0.0)
        return 0;

    root = (h - sqrt(discriminant)) / a;
    if (root <= min_t || root >= max_t) {
        root = (h + sqrt(discriminant)) / a;
        if (root <= min_t || root >= max_t)
            return 0;
    }

    hit->t = root;
    hit->point = ray_at(ray, root);
    hit->material = sphere.material;

    outward_normal = scale(subtract(hit->point, sphere.center), 1.0 / sphere.radius);
    if (dot(ray.direction, outward_normal) < 0.0)
        hit->normal = outward_normal;
    else
        hit->normal = scale(outward_normal, -1.0);

    return 1;
}

int hit_world(Ray ray, double min_t, double max_t, Hit *hit) {
    Hit current;
    int found = 0;
    int i;

    for (i = 0; i < sphere_count; i++) {
        if (hit_sphere(spheres[i], ray, min_t, max_t, &current)) {
            found = 1;
            max_t = current.t;
            *hit = current;
        }
    }

    return found;
}

int scatter(Ray incoming, Hit hit, Vec3 *attenuation, Ray *scattered) {
    Material material = materials[hit.material];
    Vec3 direction;

    if (material.type == LAMBERTIAN) {
        direction = add(hit.normal, random_unit_vector());

        if (fabs(direction.x) < 1e-8 &&
            fabs(direction.y) < 1e-8 &&
            fabs(direction.z) < 1e-8)
            direction = hit.normal;

        scattered->origin = hit.point;
        scattered->direction = direction;
        *attenuation = material.color;
        return 1;
    }

    direction = unit_vector(reflect(incoming.direction, hit.normal));
    direction = add(direction, scale(random_unit_vector(), material.fuzz));
    scattered->origin = hit.point;
    scattered->direction = direction;
    *attenuation = material.color;
    return dot(direction, hit.normal) > 0.0;
}

Vec3 ray_color(Ray ray, int depth) {
    Hit hit;
    Ray scattered;
    Vec3 attenuation;
    Vec3 direction;
    double blend;

    if (depth == 0)
        return vec3(0, 0, 0);

    if (hit_world(ray, 0.001, INFINITY, &hit)) {
        if (scatter(ray, hit, &attenuation, &scattered))
            return multiply(attenuation, ray_color(scattered, depth - 1));
        return vec3(0, 0, 0);
    }

    direction = unit_vector(ray.direction);
    blend = 0.5 * (direction.y + 1.0);
    return add(scale(vec3(1, 1, 1), 1.0 - blend),
               scale(vec3(0.5, 0.7, 1.0), blend));
}

double clamp(double value, double min, double max) {
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

void write_color(Vec3 color) {
    double r = color.x > 0.0 ? sqrt(color.x) : 0.0;
    double g = color.y > 0.0 ? sqrt(color.y) : 0.0;
    double b = color.z > 0.0 ? sqrt(color.z) : 0.0;

    printf("%d %d %d\n",
           (int)(256 * clamp(r, 0, 0.999)),
           (int)(256 * clamp(g, 0, 0.999)),
           (int)(256 * clamp(b, 0, 0.999)));
}

int main(void) {
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = (int)(image_width / aspect_ratio);
    int samples_per_pixel = 100;
    int max_depth = 50;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * image_width / image_height;
    Vec3 viewport_x = vec3(viewport_width, 0, 0);
    Vec3 viewport_y = vec3(0, -viewport_height, 0);
    Vec3 pixel_step_x = scale(viewport_x, 1.0 / image_width);
    Vec3 pixel_step_y = scale(viewport_y, 1.0 / image_height);
    Vec3 pixel00;
    int ground;
    int center;
    int left;
    int right;
    int x;
    int y;
    int sample;

    pixel00 = subtract(vec3(0, 0, -1), scale(viewport_x, 0.5));
    pixel00 = subtract(pixel00, scale(viewport_y, 0.5));
    pixel00 = add(pixel00, scale(add(pixel_step_x, pixel_step_y), 0.5));

    ground = add_material(LAMBERTIAN, vec3(0.8, 0.8, 0), 0);
    center = add_material(LAMBERTIAN, vec3(0.1, 0.2, 0.5), 0);
    left = add_material(METAL, vec3(0.8, 0.8, 0.8), 0.3);
    right = add_material(METAL, vec3(0.8, 0.6, 0.2), 1.0);

    add_sphere(vec3(0, -100.5, -1), 100, ground);
    add_sphere(vec3(0, 0, -1.2), 0.5, center);
    add_sphere(vec3(-1, 0, -1), 0.5, left);
    add_sphere(vec3(1, 0, -1), 0.5, right);

    printf("P3\n%d %d\n255\n", image_width, image_height);

    for (y = 0; y < image_height; y++) {
        for (x = 0; x < image_width; x++) {
            Vec3 pixel = vec3(0, 0, 0);

            for (sample = 0; sample < samples_per_pixel; sample++) {
                double offset_x = random_double() - 0.5;
                double offset_y = random_double() - 0.5;
                Vec3 target = pixel00;
                Ray ray;

                target = add(target, scale(pixel_step_x, x + offset_x));
                target = add(target, scale(pixel_step_y, y + offset_y));
                ray.origin = vec3(0, 0, 0);
                ray.direction = target;
                pixel = add(pixel, ray_color(ray, max_depth));
            }

            write_color(scale(pixel, 1.0 / samples_per_pixel));
        }
    }

    return 0;
}
