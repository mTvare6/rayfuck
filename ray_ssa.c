#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LAMBERTIAN 0
#define METAL 1

double random_x;
double random_y;
double random_z;

double hit_x;
double hit_y;
double hit_z;
double hit_normal_x;
double hit_normal_y;
double hit_normal_z;
double hit_t;
int hit_material;

double random_double(void) {
    return rand() / (RAND_MAX + 1.0);
}

void random_unit_vector(void) {
    double x;
    double y;
    double z;
    double length_squared;

    while (1) {
        x = 2.0 * random_double() - 1.0;
        y = 2.0 * random_double() - 1.0;
        z = 2.0 * random_double() - 1.0;
        length_squared = x * x + y * y + z * z;

        if (length_squared > 1e-160 && length_squared <= 1.0) {
            length_squared = sqrt(length_squared);
            random_x = x / length_squared;
            random_y = y / length_squared;
            random_z = z / length_squared;
            return;
        }
    }
}

int hit_sphere(
    double origin_x,
    double origin_y,
    double origin_z,
    double direction_x,
    double direction_y,
    double direction_z,
    double center_x,
    double center_y,
    double center_z,
    double radius,
    int material,
    double min_t,
    double max_t
) {
    double offset_x = center_x - origin_x;
    double offset_y = center_y - origin_y;
    double offset_z = center_z - origin_z;
    double a = direction_x * direction_x
             + direction_y * direction_y
             + direction_z * direction_z;
    double h = direction_x * offset_x
             + direction_y * offset_y
             + direction_z * offset_z;
    double c = offset_x * offset_x
             + offset_y * offset_y
             + offset_z * offset_z
             - radius * radius;
    double discriminant = h * h - a * c;
    double root;
    double outward_x;
    double outward_y;
    double outward_z;

    if (discriminant < 0.0)
        return 0;

    root = (h - sqrt(discriminant)) / a;
    if (root <= min_t || root >= max_t) {
        root = (h + sqrt(discriminant)) / a;
        if (root <= min_t || root >= max_t)
            return 0;
    }

    hit_t = root;
    hit_x = origin_x + root * direction_x;
    hit_y = origin_y + root * direction_y;
    hit_z = origin_z + root * direction_z;
    hit_material = material;

    outward_x = (hit_x - center_x) / radius;
    outward_y = (hit_y - center_y) / radius;
    outward_z = (hit_z - center_z) / radius;

    if (direction_x * outward_x
      + direction_y * outward_y
      + direction_z * outward_z < 0.0) {
        hit_normal_x = outward_x;
        hit_normal_y = outward_y;
        hit_normal_z = outward_z;
    } else {
        hit_normal_x = -outward_x;
        hit_normal_y = -outward_y;
        hit_normal_z = -outward_z;
    }

    return 1;
}

int hit_world(
    double origin_x,
    double origin_y,
    double origin_z,
    double direction_x,
    double direction_y,
    double direction_z
) {
    double closest = INFINITY;
    int found = 0;

    if (hit_sphere(origin_x, origin_y, origin_z,
                   direction_x, direction_y, direction_z,
                   0, -100.5, -1, 100, 0, 0.001, closest)) {
        closest = hit_t;
        found = 1;
    }

    if (hit_sphere(origin_x, origin_y, origin_z,
                   direction_x, direction_y, direction_z,
                   0, 0, -1.2, 0.5, 1, 0.001, closest)) {
        closest = hit_t;
        found = 1;
    }

    if (hit_sphere(origin_x, origin_y, origin_z,
                   direction_x, direction_y, direction_z,
                   -1, 0, -1, 0.5, 2, 0.001, closest)) {
        closest = hit_t;
        found = 1;
    }

    if (hit_sphere(origin_x, origin_y, origin_z,
                   direction_x, direction_y, direction_z,
                   1, 0, -1, 0.5, 3, 0.001, closest)) {
        found = 1;
    }

    return found;
}

void ray_color(
    double origin_x,
    double origin_y,
    double origin_z,
    double direction_x,
    double direction_y,
    double direction_z,
    int depth,
    double *red,
    double *green,
    double *blue
) {
    double point_x;
    double point_y;
    double point_z;
    double normal_x;
    double normal_y;
    double normal_z;
    double scattered_x;
    double scattered_y;
    double scattered_z;
    double attenuation_r;
    double attenuation_g;
    double attenuation_b;
    double fuzz;
    double reflected_x;
    double reflected_y;
    double reflected_z;
    double reflected_length;
    double projection;
    double direction_length;
    double blend;
    double throughput_r = 1.0;
    double throughput_g = 1.0;
    double throughput_b = 1.0;
    int material;
    int material_type;
    int path_length;

    for (path_length = 0; path_length < depth; path_length++) {
        if (!hit_world(origin_x, origin_y, origin_z,
                       direction_x, direction_y, direction_z)) {
            direction_length = sqrt(direction_x * direction_x
                                  + direction_y * direction_y
                                  + direction_z * direction_z);
            blend = 0.5 * (direction_y / direction_length + 1.0);
            *red = throughput_r * (1.0 - 0.5 * blend);
            *green = throughput_g * (1.0 - 0.3 * blend);
            *blue = throughput_b;
            return;
        }

        point_x = hit_x;
        point_y = hit_y;
        point_z = hit_z;
        normal_x = hit_normal_x;
        normal_y = hit_normal_y;
        normal_z = hit_normal_z;
        material = hit_material;

        if (material == 0) {
            material_type = LAMBERTIAN;
            attenuation_r = 0.8;
            attenuation_g = 0.8;
            attenuation_b = 0.0;
            fuzz = 0.0;
        } else if (material == 1) {
            material_type = LAMBERTIAN;
            attenuation_r = 0.1;
            attenuation_g = 0.2;
            attenuation_b = 0.5;
            fuzz = 0.0;
        } else if (material == 2) {
            material_type = METAL;
            attenuation_r = 0.8;
            attenuation_g = 0.8;
            attenuation_b = 0.8;
            fuzz = 0.3;
        } else {
            material_type = METAL;
            attenuation_r = 0.8;
            attenuation_g = 0.6;
            attenuation_b = 0.2;
            fuzz = 1.0;
        }

        if (material_type == LAMBERTIAN) {
            random_unit_vector();
            scattered_x = normal_x + random_x;
            scattered_y = normal_y + random_y;
            scattered_z = normal_z + random_z;

            if (fabs(scattered_x) < 1e-8
             && fabs(scattered_y) < 1e-8
             && fabs(scattered_z) < 1e-8) {
                scattered_x = normal_x;
                scattered_y = normal_y;
                scattered_z = normal_z;
            }
        } else {
            projection = direction_x * normal_x
                       + direction_y * normal_y
                       + direction_z * normal_z;
            reflected_x = direction_x - 2.0 * projection * normal_x;
            reflected_y = direction_y - 2.0 * projection * normal_y;
            reflected_z = direction_z - 2.0 * projection * normal_z;
            reflected_length = sqrt(reflected_x * reflected_x
                                  + reflected_y * reflected_y
                                  + reflected_z * reflected_z);
            random_unit_vector();
            scattered_x = reflected_x / reflected_length + fuzz * random_x;
            scattered_y = reflected_y / reflected_length + fuzz * random_y;
            scattered_z = reflected_z / reflected_length + fuzz * random_z;

            if (scattered_x * normal_x
              + scattered_y * normal_y
              + scattered_z * normal_z <= 0.0) {
                *red = 0;
                *green = 0;
                *blue = 0;
                return;
            }
        }

        throughput_r *= attenuation_r;
        throughput_g *= attenuation_g;
        throughput_b *= attenuation_b;
        origin_x = point_x;
        origin_y = point_y;
        origin_z = point_z;
        direction_x = scattered_x;
        direction_y = scattered_y;
        direction_z = scattered_z;
    }

    *red = 0;
    *green = 0;
    *blue = 0;
}

double clamp(double value) {
    if (value < 0.0)
        return 0.0;
    if (value > 0.999)
        return 0.999;
    return value;
}

int main(void) {
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = (int)(image_width / aspect_ratio);
    int samples_per_pixel = 100;
    int max_depth = 50;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * image_width / image_height;
    double pixel_step_x = viewport_width / image_width;
    double pixel_step_y = -viewport_height / image_height;
    double pixel00_x = -viewport_width / 2.0 + pixel_step_x / 2.0;
    double pixel00_y = viewport_height / 2.0 + pixel_step_y / 2.0;
    double pixel00_z = -1.0;
    double target_x;
    double target_y;
    double target_z;
    double sample_r;
    double sample_g;
    double sample_b;
    double pixel_r;
    double pixel_g;
    double pixel_b;
    int x;
    int y;
    int sample;

    printf("P3\n%d %d\n255\n", image_width, image_height);

    for (y = 0; y < image_height; y++) {
        for (x = 0; x < image_width; x++) {
            pixel_r = 0;
            pixel_g = 0;
            pixel_b = 0;

            for (sample = 0; sample < samples_per_pixel; sample++) {
                target_x = pixel00_x
                         + (x + random_double() - 0.5) * pixel_step_x;
                target_y = pixel00_y
                         + (y + random_double() - 0.5) * pixel_step_y;
                target_z = pixel00_z;

                ray_color(0, 0, 0, target_x, target_y, target_z,
                          max_depth, &sample_r, &sample_g, &sample_b);

                pixel_r += sample_r;
                pixel_g += sample_g;
                pixel_b += sample_b;
            }

            pixel_r = sqrt(pixel_r / samples_per_pixel);
            pixel_g = sqrt(pixel_g / samples_per_pixel);
            pixel_b = sqrt(pixel_b / samples_per_pixel);

            printf("%d %d %d\n",
                   (int)(256 * clamp(pixel_r)),
                   (int)(256 * clamp(pixel_g)),
                   (int)(256 * clamp(pixel_b)));
        }
    }

    return 0;
}
