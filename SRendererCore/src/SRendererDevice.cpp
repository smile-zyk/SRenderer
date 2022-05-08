#include "SRendererDevice.h"
#include <QDebug>
#include <QTime>

/********************************************************************************/
// helper functions

static inline bool JudgeOnTopLeftEdge(CoordI2D v0, CoordI2D v1)
{
    return (v0.y > v1.y) || (v0.x > v1.x && v1.y == v0.y);
}

static inline bool JudgeInsideTriangle(EdgeEquation& triEdge,VectorI3D res)
{
    bool flag = true;
    if(res.x == 0) flag &= triEdge.topLeftFlag[0];
    if(res.y == 0) flag &= triEdge.topLeftFlag[1];
    if(res.z == 0) flag &= triEdge.topLeftFlag[2];
//    return flag && res.x >= 0 && res.y >=0 && res.z >= 0;
    return flag && ((res.x >= 0 && res.y >= 0 && res.z >=0) || (res.x <= 0 && res.y <= 0 && res.z <= 0));
}

template<class T>
static inline T CorrectPerspective(float viewDepth, std::vector<T> attribute, Triangle& tri, glm::vec3& barycentric)
{
    return viewDepth * (barycentric.x * attribute[0] / tri[0].clipSpacePos.w + barycentric.y * attribute[1] / tri[1].clipSpacePos.w + barycentric.z * attribute[2] / tri[2].clipSpacePos.w);
}

template<class T>
static inline T CalculateInterpolation(T a, T b, T c, Vector3D &barycentric)
{
    return a * barycentric.x + b * barycentric.y + c * barycentric.z;
}

template<class T>
static inline T CalculateInterpolation(T a, T b, float alpha)
{
    return a * (1 - alpha) + b * alpha;
}

static inline CoordI2D CalculateInterpolation(CoordI2D a, CoordI2D b, float alpha)
{
    CoordI2D res;
    res.x = static_cast<int>(a.x * (1 - alpha) + b.x * alpha + 0.5f);
    res.y = static_cast<int>(a.y * (1 - alpha) + b.y * alpha + 0.5f);
    return res;
}

static inline Vertex CalculateInterpolation(Vertex a, Vertex b, float alpha)
{
    Vertex res;
    res.clipSpacePos = CalculateInterpolation(a.clipSpacePos, b.clipSpacePos, alpha);
    res.worldSpacePos = CalculateInterpolation(a.worldSpacePos, b.worldSpacePos, alpha);
    res.normal = CalculateInterpolation(a.normal, b.normal, alpha);
    res.texCoord = CalculateInterpolation(a.texCoord, b.texCoord, alpha);
    return res;
}

template<class T>
static inline float CalculateDistance(T point, T border)
{
    return glm::dot(point, border);
}

template<class T,size_t N>
static inline std::bitset<N> GetClipCode(T point, std::array<T, N>& clip)
{
    std::bitset<N> res;
    for(int i = 0; i < N; i++)
        if(CalculateDistance(point, clip[i]) < 0) res.set(i, 1);
    return res;
}

void SRendererDevice::ExecutePerspectiveDivision(Triangle &tri)
{
    for (int i = 0; i < 3; i++)
    {
        tri[i].ndcSpacePos.x /= tri[i].clipSpacePos.w;
        tri[i].ndcSpacePos.y /= tri[i].clipSpacePos.w;
        tri[i].ndcSpacePos.z /= tri[i].clipSpacePos.w;
    }
}

void SRendererDevice::ConvertToScreen(Triangle &tri)
{
    for (int i = 0; i < 3; i++)
    {
        tri[i].screenPos.x = static_cast<int>(0.5f * w * (tri[i].ndcSpacePos.x + 1.0f) + 0.5f);
        tri[i].screenPos.y = static_cast<int>(0.5f * h * (tri[i].ndcSpacePos.y + 1.0f) + 0.5f);
        tri[i].screenDepth = tri[i].ndcSpacePos.z;
    }
}

std::vector<Triangle> ConstructTriangle(std::vector<Vertex> vertexList)
{
    std::vector<Triangle> res;
    for(int i = 0; i < vertexList.size() - 2; i++)
    {
        int k = (i + 1) % vertexList.size();
        int m = (i + 2) % vertexList.size();
        Triangle tri{vertexList[0], vertexList[k], vertexList[m]};
        res.push_back(tri);
    }
    return res;
}

Fragment ConstructFragment(int x, int y, float z, float viewDepth, Triangle& tri, Vector3D& barycentric)
{
    Fragment frag;
    frag.screenPos.x = x;
    frag.screenPos.y = y;
    frag.screenDepth = z;
    frag.worldSpacePos = CorrectPerspective(viewDepth, std::vector<Color>{tri[0].worldSpacePos, tri[1].worldSpacePos, tri[2].worldSpacePos}, tri, barycentric);
    frag.normal = CorrectPerspective(viewDepth, std::vector<Vector3D>{tri[0].normal, tri[1].normal, tri[2].normal}, tri, barycentric);
    frag.texCoord = CorrectPerspective(viewDepth, std::vector<Coord2D>{tri[0].texCoord, tri[1].texCoord, tri[2].texCoord}, tri, barycentric);
    return frag;
}

CoordI4D SRendererDevice::GetBoundingBox(Triangle & tri)
{
    int xMin = w - 1;
    int yMin = h - 1;
    int xMax = 0;
    int yMax = 0;
    for(int i = 0; i < 3; i++)
    {
        xMin = std::min(xMin, tri[i].screenPos.x);
        yMin = std::min(yMin, tri[i].screenPos.y);
        xMax = std::max(xMax, tri[i].screenPos.x);
        yMax = std::max(yMax, tri[i].screenPos.y);
    }
    return {
        xMin > 0 ? xMin : 0,
        yMin > 0 ? yMin : 0,
        xMax < w - 1 ? xMax : w - 1,
        yMax < h - 1 ? yMax : h - 1};
}

/********************************************************************************/
// struct EdgeEquation implementation
// see in https://zhuanlan.zhihu.com/p/140926917

EdgeEquation::EdgeEquation(const Triangle &tri)
{
    I = {
        tri[0].screenPos.y - tri[1].screenPos.y,
        tri[1].screenPos.y - tri[2].screenPos.y,
        tri[2].screenPos.y - tri[0].screenPos.y};
    J = {
        tri[1].screenPos.x - tri[0].screenPos.x,
        tri[2].screenPos.x - tri[1].screenPos.x,
        tri[0].screenPos.x - tri[2].screenPos.x};
    K = {
        tri[0].screenPos.x * tri[1].screenPos.y - tri[0].screenPos.y * tri[1].screenPos.x,
        tri[1].screenPos.x * tri[2].screenPos.y - tri[1].screenPos.y * tri[2].screenPos.x,
        tri[2].screenPos.x * tri[0].screenPos.y - tri[2].screenPos.y * tri[0].screenPos.x};
    topLeftFlag[0] = JudgeOnTopLeftEdge(tri[0].screenPos,tri[1].screenPos);
    topLeftFlag[1] = JudgeOnTopLeftEdge(tri[1].screenPos,tri[2].screenPos);
    topLeftFlag[2] = JudgeOnTopLeftEdge(tri[2].screenPos,tri[0].screenPos);
    twoArea = K[0] + K[1] + K[2];
    delta = 1.f / twoArea;
}

VectorI3D EdgeEquation::GetResult(int x, int y)
{
    VectorI3D res = I * x + J * y + K;
    return res;
}

void EdgeEquation::UpX(VectorI3D& res)
{
    res += I;
}

void EdgeEquation::UpY(VectorI3D& res)
{
    res += J;
}

Vector3D EdgeEquation::GetBarycentric(VectorI3D val)
{
    return {val.y * delta, val.z * delta, val.x * delta};
}

/********************************************************************************/
// initiaize clip plane and screen border

SRendererDevice::SRendererDevice(int _w, int _h):shader(nullptr), w(_w), h(_h), frameBuffer(w, h)
{
    {// set view planes
        // near
        viewPlanes[0] = {0, 0, 1.f, 1.f};
        // far
        viewPlanes[1] = {0, 0, -1.f, 1.f};
        // left
        viewPlanes[2] = {1.f, 0, 0, 1.f};
        // right
        viewPlanes[3] = {-1.f, 0, 0, 1.f};
        // top
        viewPlanes[4] = {0, 1.f, 0, 1.f};
        // bottom
        viewPlanes[5] = {0, -1.f, 0, 1.f};
    }
    {// set screen border
        // left
        screenLines[0] = {1.f, 0, 0};
        // right
        screenLines[1] = {-1.f, 0, (float)w};
        // bottom
        screenLines[2] = {0, 1.f, 0};
        // top
        screenLines[3] = {0, -1.f, (float)h};
    }
}

/********************************************************************************/
// half-space triangle rasterization algorithm

void SRendererDevice::RasterizationTriangle(Triangle &tri)
{
    CoordI4D boundingBox = GetBoundingBox(tri);
    int xMin = boundingBox[0];
    int yMin = boundingBox[1];
    int xMax = boundingBox[2];
    int yMax = boundingBox[3];
    EdgeEquation triEdge(tri);
    if(faceCulling && triEdge.twoArea <= 0) return;
    else if(triEdge.twoArea == 0) return;
    Fragment frag;
    VectorI3D cy = triEdge.GetResult(xMin, yMin);
    for(int y = yMin; y <= yMax; y++)
    {
        VectorI3D cx = cy;
        for(int x = xMin; x <= xMax; x++)
        {
            if(JudgeInsideTriangle(triEdge, cx))
            {
                Vector3D barycentric = triEdge.GetBarycentric(cx);
                float screenDepth = CalculateInterpolation(tri[0].screenDepth, tri[1].screenDepth, tri[2].screenDepth, barycentric);
                if (frameBuffer.JudgeDepth(x, y, screenDepth))
                {
                    float viewDepth = 1.0f / (barycentric.x / tri[0].ndcSpacePos.w + barycentric.y / tri[1].ndcSpacePos.w + barycentric.z / tri[2].ndcSpacePos.w);
                    frag = ConstructFragment(x, y, screenDepth, viewDepth, tri, barycentric);
                    shader->FragmentShader(frag);
                    frameBuffer.SetPixel(frag.screenPos.x,frag.screenPos.y,frag.fragmentColor);
                }
            }
            triEdge.UpX(cx);
        }
        triEdge.UpY(cy);
    }
}

/********************************************************************************/
// Bresenham's line algorithm

void SRendererDevice::DrawLine(Line& line)
{
    int x0 = glm::clamp(static_cast<int>(line[0].x), 0, w - 1);
    int x1 = glm::clamp(static_cast<int>(line[1].x), 0, w - 1);
    int y0 = glm::clamp(static_cast<int>(line[0].y), 0, h - 1);
    int y1 = glm::clamp(static_cast<int>(line[1].y), 0, h - 1);
    bool steep = false;
    if (abs(x0 - x1) < abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int k = dy > 0 ? 1 : -1;
    if (dy < 0)dy = -dy;
    float e = -dx;
    int x = x0, y = y0;
    while (x != x1)
    {
        if (steep)frameBuffer.SetPixel(y, x, lineColor);
        else frameBuffer.SetPixel(x, y, lineColor);
        e += (2 * dy);
        if (e > 0)
        {
            y += k;
            e -= (2 * dx);
        }
        ++x;
    }
}

/********************************************************************************/
// Cohen-Sutherland algorithm in screen space

std::optional<Line> SRendererDevice::ClipLine(Line& line)
{
    std::bitset<4> code[2] =
    {
        GetClipCode(Coord3D(line[0], 1), screenLines),
        GetClipCode(Coord3D(line[1], 1), screenLines)
    };
    if((code[0] | code[1]).none())return line;
    if((code[0] & code[1]).any())return std::nullopt;
    for(int i = 0; i < 4; i++)
    {
        if((code[0] ^ code[1])[i])
        {
            float da = CalculateDistance(Coord3D(line[0], 1), screenLines[i]);
            float db = CalculateDistance(Coord3D(line[1], 1), screenLines[i]);
            float alpha = da / (da - db);
            CoordI2D np = CalculateInterpolation(line[0], line[1], alpha);
            if(da > 0)
            {
                line[1] = np;
                code[1] = GetClipCode(Coord3D(np, 1), screenLines);
            }
            else
            {
                line[0] = np;
                code[0] = GetClipCode(Coord3D(np, 1), screenLines);
            }
        }
    }
    return line;
}

/********************************************************************************/
// WireframedTriangle

void SRendererDevice::WireframedTriangle(Triangle &tri)
{
    Line triLine[3] =
    {
        {tri[0].screenPos, tri[1].screenPos},
        {tri[1].screenPos, tri[2].screenPos},
        {tri[2].screenPos, tri[0].screenPos},
    };
    for(auto &line : triLine)
    {
        auto res = ClipLine(line);
        if(res) DrawLine(*res);
    }
}

/********************************************************************************/
// render vertex

void SRendererDevice::PointedTriangle(Triangle &tri)
{
    for(int i = 0; i < 3; i++)
    {
        if(tri[i].screenPos.x >= 0 && tri[i].screenPos.x <= w - 1 &&
                tri[i].screenPos.y >= 0 && tri[i].screenPos.y <= h - 1 &&
                tri[i].screenDepth <= 1.f)
        {
            frameBuffer.SetPixel(tri[i].screenPos.x, tri[i].screenPos.y,
                                 pointColor);
        }
    }
}

/********************************************************************************/
// clip triangle
// Cohen-Sutherland algorithm & Sutherland-Hodgman algorithm in homogeneous space

std::vector<Triangle> SRendererDevice::ClipTriangle(Triangle &tri)
{
    std::bitset<6> code[3] =
    {
        GetClipCode(tri[0].clipSpacePos, viewPlanes),
        GetClipCode(tri[1].clipSpacePos, viewPlanes),
        GetClipCode(tri[2].clipSpacePos, viewPlanes)
    };
    if((code[0] | code[1] | code[2]).none())
        return {tri};
    if((code[0] & code[1] & code[2]).any())
        return {};
    if(((code[0] ^ code[1])[0]) || ((code[1] ^ code[2])[0]) || ((code[2] ^ code[0])[0])) // intersects near plane
    {
        std::vector<Vertex> res;
        for(int i = 0; i < 3; i++)
        {
            int k = (i + 1) % 3;
            if(!code[i][0] && !code[k][0])
            {
                res.push_back(tri[k]);
            }
            else if(!code[i][0] && code[k][0])
            {
                float da = CalculateDistance(tri[i].clipSpacePos, viewPlanes[0]);
                float db = CalculateDistance(tri[k].clipSpacePos, viewPlanes[0]);
                float alpha = da / (da - db);
                Vertex np = CalculateInterpolation(tri[i], tri[k], alpha);
                res.push_back(np);
            }
            else if(code[i][0] && !code[k][0])
            {
                float da = CalculateDistance(tri[i].clipSpacePos, viewPlanes[0]);
                float db = CalculateDistance(tri[k].clipSpacePos, viewPlanes[0]);
                float alpha = da / (da - db);
                Vertex np = CalculateInterpolation(tri[i], tri[k], alpha);
                res.push_back(np);
                res.push_back(tri[k]);
            }
        }
        return ConstructTriangle(res);
    }
    return std::vector<Triangle>{tri};
}

/********************************************************************************/
// process triangle
// clip triangle and choose one mode {TRIANGLE,LINE,POINT} to render.

void SRendererDevice::ProcessTriangle(Triangle &tri)
{
    for (int i = 0; i < 3; i++)
    {
        shader->VertexShader(tri[i]);
    }
    std::vector<Triangle> completedTriangleList = ClipTriangle(tri);
    for (auto &ctri : completedTriangleList)
    {
        ExecutePerspectiveDivision(ctri);
        ConvertToScreen(ctri);
        if(renderMode == FACE)RasterizationTriangle(ctri);
        else if(renderMode == EDGE) WireframedTriangle(ctri);
        else if(renderMode == VERTEX) PointedTriangle(ctri);
    }
}

/********************************************************************************/
// main render function

void SRendererDevice::Render()
{
    std::vector<Triangle> triangleList;
    for (int i = 0; i < indices.size(); i += 3)
    {
        assert(i + 1 < indices.size() && i + 2 < indices.size());
        triangleList.push_back({vertexList.at(indices.at(i)), vertexList.at(indices.at(i + 1)), vertexList.at(indices.at(i + 2))});
    }
    if(multiThread)
    {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, triangleList.size()),
        [&](tbb::blocked_range<size_t> r)
        {
            for(size_t i = r.begin(); i < r.end(); i++)
                ProcessTriangle(triangleList[i]);
        });
    }
    else
    {
        for(int i = 0; i < triangleList.size(); i++)
            ProcessTriangle(triangleList[i]);
    }
}
