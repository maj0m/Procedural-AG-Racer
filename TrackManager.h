#pragma once
#include "framework.h"
#include "renderstate.h"

struct TrackSegment {
	vec4 start_r; // a.x,a.y,a.z,r
	vec4 end_pad; // b.x,b.y,b.z,0
};

class TrackManager {
public:
	std::vector<TrackSegment> segments;
	GLuint segmentsSSBO = 0;   // binding = 4
	GLuint segmentCount = 0;

	// Only create after TerrainData UBO is uploaded!! (if using collision compute shader)
	TrackManager() {
		glGenBuffers(1, &segmentsSSBO);

		GenerateSegments();
		BindSegmentSSBO();
	}

	~TrackManager() {
		if (segmentsSSBO) glDeleteBuffers(1, &segmentsSSBO);
	}

    void GenerateSegments() {
        segments.clear();

        // Params
        const int   INITIAL_POINT_COUNT = 8;
        const float SPREAD_FROM_CENTER = 1000.0f;
        const float MAX_MIDPOINT_DISP = 75.0f;
        const float MIN_HULLPOINT_DISTANCE = 150.0f;
        const float TRACK_RADIUS = 60.0f;
        const int   CATMULL_SEGMENTS = 20;

        // 1) Random base points
        std::vector<vec2> pts;
        pts.reserve(INITIAL_POINT_COUNT);
        for (int i = 0; i < INITIAL_POINT_COUNT; ++i)
            pts.emplace_back(randf(-SPREAD_FROM_CENTER, SPREAD_FROM_CENTER), randf(-SPREAD_FROM_CENTER, SPREAD_FROM_CENTER));
        if (pts.size() < 3) return;

        // 2) Convex hull (monotone chain)
        std::sort(pts.begin(), pts.end(), [](const vec2& a, const vec2& b) {
            return (a.x == b.x) ? (a.y < b.y) : (a.x < b.x);
            });
        std::vector<vec2> lower, upper;
        for (const vec2& p : pts) {
            while (lower.size() >= 2 && cross(lower.back() - lower[lower.size() - 2], p - lower.back()) <= 0.0f) lower.pop_back();
            lower.push_back(p);
        }
        for (int i = int(pts.size()) - 1; i >= 0; --i) {
            const vec2& p = pts[i];
            while (upper.size() >= 2 && cross(upper.back() - upper[upper.size() - 2], p - upper.back()) <= 0.0f) upper.pop_back();
            upper.push_back(p);
        }
        if (!lower.empty()) lower.pop_back();
        if (!upper.empty()) upper.pop_back();

        std::vector<vec2> hull;
        hull.reserve(lower.size() + upper.size());
        hull.insert(hull.end(), lower.begin(), lower.end());
        hull.insert(hull.end(), upper.begin(), upper.end());
        if (hull.size() < 3) return;

        // 3) Displaced midpoints
        std::vector<vec2> midHull;
        midHull.reserve(hull.size() * 2);
        int N = int(hull.size());
        for (int i = 0; i < N; ++i) {
            const vec2& P0 = hull[i];
            const vec2& P1 = hull[(i + 1) % N];
            vec2 mid = (P0 + P1) * 0.5f;
            vec2 dir = P1 - P0;
            vec2 nrm = normalize(vec2(dir.y, dir.x));
            mid += nrm * randf(-1.0f, 1.0f) * MAX_MIDPOINT_DISP;
            midHull.push_back(P0);
            midHull.push_back(mid);
        }
        hull.swap(midHull);
        N = int(hull.size());

        // 4) Remove too-close points (circular)
        std::vector<vec2> cleaned;
        cleaned.reserve(N);
        for (int i = 0; i < N; ++i) {
            if (cleaned.empty() || length(hull[i] - cleaned.back()) >= MIN_HULLPOINT_DISTANCE)
                cleaned.push_back(hull[i]);
        }
        if (cleaned.size() >= 2 && length(cleaned.back() - cleaned.front()) < MIN_HULLPOINT_DISTANCE)
            cleaned.pop_back();

        hull.swap(cleaned);
        N = int(hull.size());
        if (N < 4) {
            for (int i = 0; i < N; ++i) {
                vec3 a(hull[i].x, 0.0f, hull[i].y);
                vec3 b(hull[(i + 1) % N].x, 0.0f, hull[(i + 1) % N].y);
                segments.push_back({ vec4(a.x, a.y, a.z, TRACK_RADIUS), vec4(b.x, b.y, b.z, 0.0f) });
            }
            return;
        }

        // 5) Catmull–Rom sampling (closed loop)
        auto catmull = [](const vec2& Pm1, const vec2& P0, const vec2& P1, const vec2& P2, float t) {
            float t2 = t * t, t3 = t2 * t;
            return vec2(
                0.5f * ((2 * P0.x) + (-Pm1.x + P1.x) * t + (2 * Pm1.x - 5 * P0.x + 4 * P1.x - P2.x) * t2 + (-Pm1.x + 3 * P0.x - 3 * P1.x + P2.x) * t3),
                0.5f * ((2 * P0.y) + (-Pm1.y + P1.y) * t + (2 * Pm1.y - 5 * P0.y + 4 * P1.y - P2.y) * t2 + (-Pm1.y + 3 * P0.y - 3 * P1.y + P2.y) * t3)
            );
        };

        std::vector<vec3> path;
        path.reserve(N * (CATMULL_SEGMENTS + 1));
        for (int i = 0; i < N; ++i) {
            const vec2& Pm1 = hull[(i - 1 + N) % N];
            const vec2& P0 = hull[i];
            const vec2& P1 = hull[(i + 1) % N];
            const vec2& P2 = hull[(i + 2) % N];
            for (int s = 0; s <= CATMULL_SEGMENTS; ++s) {
                if (i > 0 && s == 0) continue; // avoid duplicates at span joins
                float t = float(s) / float(CATMULL_SEGMENTS);
                vec2 p = catmull(Pm1, P0, P1, P2, t);
                path.emplace_back(p.x, 0.0f, p.y);
            }
        }
        if (!path.empty()) path.push_back(path.front());

        // 6) Emit segments
        for (size_t i = 0; i + 1 < path.size(); ++i) {
            const vec3& a = path[i];
            const vec3& b = path[i + 1];
            segments.push_back({ vec4(a.x, a.y, a.z, TRACK_RADIUS), vec4(b.x, b.y, b.z, 0.0f) });
        }
    }

	void BindSegmentSSBO() {
		segmentCount = (GLuint)segments.size();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, segmentsSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TrackSegment) * segmentCount, segments.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, segmentsSSBO); // binding = 4
	}

	// Return indices that intersect a chunk AABB
	void GetSegmentsForChunk(const vec3& chunkId, float chunkSize, std::vector<int>& outIndices) const {
		outIndices.clear();
		
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                // chunk AABB
                vec3 minC = (chunkId + vec3((float)x, 0.0, (float)y)) * chunkSize;
                vec3 maxC = minC + vec3(chunkSize, chunkSize, chunkSize);

                for (int i = 0; i < segments.size(); ++i) {
                    vec3 start = vec3(segments[i].start_r.x, segments[i].start_r.y, segments[i].start_r.z);
                    vec3 end = vec3(segments[i].end_pad.x, segments[i].end_pad.y, segments[i].end_pad.z);
                    float radius = segments[i].start_r.w;

                    // Expand segment’s AABB by radius
                    vec3 lo = minVec3(start, end) - vec3(radius);
                    vec3 hi = maxVec3(start, end) + vec3(radius);

                    // AABB overlap test
                    bool overlap = (lo.x <= maxC.x && hi.x >= minC.x)
                        && (lo.y <= maxC.y && hi.y >= minC.y)
                        && (lo.z <= maxC.z && hi.z >= minC.z);
                    if (overlap) outIndices.push_back(i);
                }
            }
        }

		//printf("Total segments: %d --- Segments for Chunk[%f : %f] - %d\n", segments.size(), chunkId.x, chunkId.z, outIndices.size());
	}
};