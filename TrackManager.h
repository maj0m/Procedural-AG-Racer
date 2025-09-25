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

	GLuint segmentsSSBO = 0;   // binding = 4 (example)
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

		vec3 prevPos = vec3(0.0, 0.0, 0.0);

		for (int i = 0; i < 100; i++) {
			vec3 pos = vec3(random(-10000.0, 10000.0), 0.0, random(-10000.0, 10000.0));

			TrackSegment seg = {
				vec4(prevPos.x, prevPos.y, prevPos.z, 50.0),
				vec4(pos.x, pos.y, pos.z, 0.0)
			};

			segments.push_back(seg);
			prevPos = pos;
		}


		
	}

	void BindSegmentSSBO() {
		segmentCount = (GLuint)segments.size();

		// Segment payload
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, segmentsSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TrackSegment) * segmentCount, segments.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, segmentsSSBO); // binding = 4
	}

	// Fast coarse filter: return indices that intersect a chunk AABB
	void GetSegmentsForChunk(const vec3& chunkId, float chunkSize, std::vector<int>& outIndices) const {
		outIndices.clear();
		// chunk AABB
		vec3 minC = chunkId * chunkSize;
		vec3 maxC = minC + vec3(chunkSize, chunkSize, chunkSize);

		for (int i = 0; i < segments.size(); ++i) {
			const auto& s = segments[i];
			vec3 start = vec3(s.start_r.x, s.start_r.y, s.start_r.z);
			vec3 end = vec3(s.end_pad.x, s.end_pad.y, s.end_pad.z);
			float radius = s.start_r.w;

			// Expand segment’s AABB by radius
			vec3 lo = minVec3(start, end) - vec3(radius);
			vec3 hi = maxVec3(start, end) + vec3(radius);
			// AABB overlap test
			bool overlap = (lo.x <= maxC.x && hi.x >= minC.x)
				&& (lo.y <= maxC.y && hi.y >= minC.y)
				&& (lo.z <= maxC.z && hi.z >= minC.z);
			if (overlap) outIndices.push_back(i);
		}

		//printf("Total segments: %d --- Segments for Chunk[%f : %f] - %d\n", segments.size(), chunkId.x, chunkId.z, outIndices.size());
	}
};