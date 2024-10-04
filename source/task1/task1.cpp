#include <limits>

#include "Scene.h"

#include "task1.h"

constexpr float epsilon = 0.001f;

bool intersectRayPlane(const float3 &p, const float3 &d, const float4 &plane, float &t)
{
    float denom = dot(d, float3(plane.x, plane.y, plane.z));
    float temp;
    if (fabs(denom) == 0) {
        return false;
    }

    temp = (-dot(p, float3(plane.x, plane.y, plane.z)) + plane.w) / denom;

    // If t neg its behind
    if (temp < epsilon)
        return false;
    t = temp;
    return true;
}

bool intersectsRayPlane(const float3 &p, const float3 &d, const Plane *planes, std::size_t num_planes, float t_min, float t_max)
{

    for (std::size_t i = 0; i < num_planes; ++i)
    {
        float t;
        if (intersectRayPlane(p, d, planes[i].p, t))
        {
            if (t >= t_min && t <= t_max)
                return true;
        }
    }
    return false;


}

const Plane *findClosestHitPlanes(const float3 &p, const float3 &d, const Plane *planes, std::size_t num_planes, float &t)
{
    // TODO: implement intersection test between a ray and a set of planes.
    // This function should find the CLOSEST intersection with a plane along
    // the ray. The ray is given by its start point p and direction d.
    // A plane is defined by the plane normal n and the offset w along the normal.
    // Each plane in planes contains a float4 member p where the plane normal n is
    // (p.x, p.y, p.z) and w is p.w.
    // If an intersection is found, set t to the ray parameter and
    // return a pointer to the hit plane.
    // If no intersection is found, return nullptr.
    const Plane *closest_plane = nullptr;
    float closest_t = std::numeric_limits<float>::max(); // Initialize with a large value

    for (std::size_t i = 0; i < num_planes; ++i)
    {
        float temp_t;
        if (intersectRayPlane(p, d, planes[i].p, temp_t))
        {
            if (temp_t < closest_t)
            {
                closest_t = temp_t;
                closest_plane = &planes[i];
            }
        }
    }

    if (closest_plane)
        t = closest_t; // Set t

    return closest_plane;

}

bool intersectRayTriangle(const float3 &p, const float3 &d, const float3 &p1,
                          const float3 &p2, const float3 &p3, float &t,
                          float &lambda_1, float &lambda_2)
{
    // Triange calcs
    float3 e1 = p2 - p1;
    float3 e2 = p3 - p1;
    float3 h = cross(d, e2);
    float temp;
    float u, v;
    float a = dot(e1, h);

    // maybe need this
    //if (fabs(a) < epsilon) // Yeeee i dont need it
      //  return false;

    float f = 1.0 / a;
    float3 s = p - p1; // q
    u = f * dot(s, h);

    // lamda 1 check
    if (u < 0.0f || u > 1.0f)
        return false;

    float3 q = cross(s, e1);
    v = f * dot(d, q);

    // lamda 2 check
    if (v < 0.0f || u + v > 1.0f)
        return false;

    // Compute the intersection parameter t
    temp = f * dot(e2, q);

    if (temp < epsilon)
        return false;
    t = temp;
    lambda_1 = u;
    lambda_2 = v;
    return true;
}

const Triangle *findClosestHitTriangles(const float3 &p, const float3 &d,
                                        const Triangle *triangles,
                                        std::size_t num_triangles,
                                        const float3 *vertices, float &t,
                                        float &lambda_1, float &lambda_2)
{
    // TODO: implement intersection test between a ray and a set of triangles.
    // This function should find the CLOSEST intersection with a triangle along
    // the ray. The ray is given by its start point p and direction d. A triangle
    // is represented as an array of three vertex indices. The position of each
    // vertex can be looked up from the vertex array via the vertex index.
    // triangles points to the first element of an array of num_triangles
    // triangles. If an intersection is found, set t to the ray parameter and
    // lambda_1 and lambda_2 to the barycentric coordinates corresponding to the
    // closest point of intersection, and return a pointer to the hit triangle.
    // If no intersection is found, return nullptr.

    float local_lambda_1, local_lambda_2, local_t;

    float lamda_1_temp, lamda_2_temp;
    float local_t_low;
    float temp_t;
    local_t_low = std::numeric_limits<float>::max();
    int return_index = -1;
    for (size_t i = 0; i < num_triangles; i++) {
        if(intersectRayTriangle(p, d, vertices[triangles[i][0]], vertices[triangles[i][1]], vertices[triangles[i][2]], local_t, local_lambda_1, local_lambda_2))
        {
            if (local_t_low > local_t) {
                local_t_low = local_t;
                lamda_1_temp = local_lambda_1;
                lamda_2_temp = local_lambda_2;
                temp_t = local_t;
                return_index = i;
            }
        }
    }
    if(return_index >= 0)
    {

        lambda_1 = lamda_1_temp;
        lambda_2 = lamda_2_temp;
        t = temp_t;
        return &triangles[return_index];
    }
    return nullptr;
}

bool intersectsRayTriangle(const float3 &p, const float3 &d,
                           const Triangle *triangles, std::size_t num_triangles,
                           const float3 *vertices, float t_min, float t_max)
{
    // TODO: implement intersection test between a ray and a set of triangles.
    // This method only has to detect whether there is an intersection with ANY
    // triangle along the given subset of the ray. The ray is given by its start
    // point p and direction d. A triangle is represented as an array of three
    // vertex indices. The position of each vertex can be looked up from the
    // vertex array via the vertex index. triangles points to an array of
    // num_triangles. If an intersection is found that falls on a point on the ray
    // between t_min and t_max, return true. Otherwise, return false.
    float local_lambda_1, local_lambda_2, local_t;
    for (std::size_t i = 0; i < num_triangles; ++i) {
        if(intersectRayTriangle(p, d, vertices[triangles[i][0]], vertices[triangles[i][1]], vertices[triangles[i][2]], local_t, local_lambda_1, local_lambda_2))
        {
            if (local_t >= t_min && local_t <= t_max)
                return true;
        }
    }

    return false;
}

float3 reflect(const float3 &I, const float3 &N) {
    return I - 2.0f * dot(I, N) * N;
}



float3 shade(const float3& p, const float3& d, const HitPoint& hit,
             const Scene& scene, const Pointlight* lights,
             std::size_t num_lights) {
    float3 color = {0.0f, 0.0f, 0.0f};
    float3 v = normalize(p - hit.position);
    float3 n = hit.normal;

    for (std::size_t i = 0; i < num_lights; ++i) {
        float3 l = normalize(lights[i].position - hit.position);
        // Check for shadow
        float3 shadow_start = hit.position + epsilon * n;
        float light_distance = length(lights[i].position - hit.position);
        if (!scene.intersectsRay(shadow_start, l, epsilon, light_distance - epsilon)) {


            // diffuse one
            float ndotl = fmax(dot(n, l), 0.0f);
            float3 cd = hit.k_d * ndotl;

            // spectacular
            float3 r = normalize(reflect(-l, n));

            float rdotv = fmax(dot(r, v), 0.0f);
            float3 cs = (float)pow(rdotv, hit.m) * hit.k_s  ;

            if (std::isinf(hit.m)) continue;
            // color calc here
            color.x += (cd.x + cs.x) * lights[i].color.x;
            color.y += (cd.y + cs.y) * lights[i].color.y;
            color.z += (cd.z + cs.z) * lights[i].color.z;


        }
    }

    return color;
}


float3 shade2(const float3& p, const float3& d, const HitPoint& hit,
             const Scene& scene, const Pointlight* lights,
             std::size_t num_lights, int max_bounces, const float3& background_color) {
    float3 color = {0.0f, 0.0f, 0.0f};
    float3 v = normalize(p - hit.position);
    float3 n = hit.normal;

    // This function is simply here for the bonus task. You can ignore it for the basic task

    if (max_bounces > 0 && std::isinf(hit.m)) {
        // reflection of the original ray
        float3 reflection_direction = normalize(reflect(d, n));

        float3 new_origin = hit.position + epsilon * n;
        std::optional<HitPoint> hit2 = scene.findClosestHit(new_origin, reflection_direction);
        // Recursively trace the reflected ray
        if(hit2)
        {
            for (std::size_t i = 0; i < num_lights; ++i) {
                float3 l = normalize(lights[i].position - hit2.value().position);
                float light_distance = length(lights[i].position - hit.position);

                if (!scene.intersectsRay(new_origin, l, epsilon, light_distance - epsilon)) {
                    float3 trace_val = shade2(new_origin, reflection_direction, hit2.value(), scene, lights,
                                             num_lights, max_bounces - 1, background_color);
                    color.x += hit.k_s.x * trace_val.x;
                    color.y += hit.k_s.y * trace_val.y;
                    color.z += hit.k_s.z * trace_val.z;
                    return color;
                }
            }

        } else {

            color.x += hit.k_s.x * background_color.x;
            color.y += hit.k_s.y * background_color.y;
            color.z += hit.k_s.z * background_color.z;
            return color;
        }

    }


    for (std::size_t i = 0; i < num_lights; ++i) {
        float3 l = normalize(lights[i].position - hit.position);
        // Check for shadow
        float3 shadow_start = hit.position + epsilon * n;
        float light_distance = length(lights[i].position - hit.position);
        if (!scene.intersectsRay(shadow_start, l, epsilon, light_distance - epsilon)) {


            // diffuse one
            float ndotl = fmax(dot(n, l), 0.0f);
            float3 cd = hit.k_d * ndotl;

            // spectacular
            float3 r = normalize(reflect(-l, n));

            float rdotv = fmax(dot(r, v), 0.0f);
            float3 cs = (float)pow(rdotv, hit.m) * hit.k_s  ;

            if (std::isinf(hit.m)) continue;
            // color calc here
            color.x += (cd.x + cs.x) * lights[i].color.x;
            color.y += (cd.y + cs.y) * lights[i].color.y;
            color.z += (cd.z + cs.z) * lights[i].color.z;
        }
    }

    return color;
}




void render(image2D<float3>& framebuffer, int left, int top, int right,
            int bottom, const Scene& scene, const Camera& camera,
            const Pointlight* lights, std::size_t num_lights,
            const float3& background_color, int max_bounces) {

    // Calculate the dimensions of the image plane


    float3 w = normalize(camera.eye - camera.lookat);
    float3 u = normalize(cross(camera.up, w));
    float3 v = cross(w, u);


    for (int y = top; y < bottom; ++y) {
        for (int x = left; x < right; ++x) {
            float ratio =  static_cast<float>(width(framebuffer)) / static_cast<float>(height(framebuffer)) ;
            float image_plane_height = camera.w_s / ratio; //

            float pixel_ndc_x = (x + (camera.w_s / static_cast<float>(width(framebuffer))) + 0.5f - static_cast<float>(width(framebuffer)) / 2) * (camera.w_s / static_cast<float>(width(framebuffer)));
            float pixel_ndc_y = (y + image_plane_height /static_cast<float>(height(framebuffer)) + 0.5f - static_cast<float>(height(framebuffer)) /2) * (image_plane_height/static_cast<float>(height(framebuffer)));
            float3 pixel_world_position = pixel_ndc_x * u - pixel_ndc_y * v - camera.f * w;
            float3 ray_direction = normalize(pixel_world_position);
            std::optional<HitPoint> hit = scene.findClosestHit(camera.eye, ray_direction);

            if (hit) {
                framebuffer(x, y) = shade2(camera.eye, ray_direction, hit.value(), scene, lights, num_lights, max_bounces, background_color);
            } else {
                // If no object is hit, use the background color
                framebuffer(x, y) = background_color;
            }
        }
    }
}
