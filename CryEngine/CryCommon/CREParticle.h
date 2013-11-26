#include DEVIRTUALIZE_HEADER_FIX(CREParticle.h)

#ifndef __CREPARTICLE_H__
#define __CREPARTICLE_H__

#include "CryThread.h"
#include <IJobManager.h>
#include <MemoryAccess.h>

// forward declarations
class CREParticle;

#define SVF_Particle		SVF_P3F_C4B_T4B_N3F2			// using typedef is problematic for CryCG

struct SRenderVertices
{
	FixedDynArray<SVF_Particle>			aVertices;
	FixedDynArray<uint16>						aIndices;
	int															nBaseVertexIndex;
	float														fMaxPixels;
	float														fPixels;
	
	SRenderVertices()
		{ memset(this, 0, sizeof(*this)); }

#if defined(PS3)
	virtual ~SRenderVertices() { /* needed to enforce correct layout from CryCG*/ } 
#endif

	inline void CopyVertices( Array<SVF_Particle>& aSrc )
	{
		int nVerts = min(aSrc.size(), aVertices.available());
		cryMemcpy(aVertices.grow_raw(nVerts), &aSrc[0], sizeof(SVF_Particle) * nVerts);
		aSrc.erase_front(nVerts);
	}

	inline void ExpandQuadVertices() 
	{ 
		ExpandQuadVerticesImpl(aVertices.grow_raw(3));
	}

	inline void ExpandQuadVerticesImpl(SVF_Particle* aV)
	{
		aV[3] = aV[2] = aV[1] = aV[0];

		aV[1].st.x = 255;
		aV[2].st.y = 255;
		aV[3].st.x = 255;
		aV[3].st.y = 255;
	}

	inline void ExpandQuadVertices(const SVF_Particle& RESTRICT_REFERENCE part)
	{
		ExpandQuadVerticesImpl(part, aVertices.grow_raw(4));
	}

	inline void ExpandQuadVerticesImpl(const SVF_Particle& RESTRICT_REFERENCE part, SVF_Particle* aV)
	{		
		CryPrefetch(aV + 4);

		const uint32* s = reinterpret_cast<const uint32*>(&part);
		uint32* a = reinterpret_cast<uint32*>(aV);
		uint32* b = reinterpret_cast<uint32*>(aV + 1);
		uint32* c = reinterpret_cast<uint32*>(aV + 2);
		uint32* d = reinterpret_cast<uint32*>(aV + 3);

		for (size_t i = 0; i < sizeof(SVF_Particle) / sizeof(uint32); ++ i)
		{
			uint32 v = s[i];
			a[i] = v;
			b[i] = v;
			c[i] = v;
			d[i] = v;
		}

		aV[1].st.x = 255;
		aV[2].st.y = 255;
		aV[3].st.x = 255;
		aV[3].st.y = 255;
	}

	inline void ExpandOctVertices()
	{
		ExpandOctVerticesImpl( aVertices.grow_raw(7) - 1 );
	}

	inline void ExpandOctVerticesImpl(SVF_Particle* aV)
	{
		aV[7] = aV[6] = aV[5] = aV[4] = aV[3] = aV[2] = aV[1] = aV[0];

		aV[0].st.x = 75;
		aV[0].st.y = 0;
		aV[1].st.x = 180;
		aV[1].st.y = 0;
		aV[2].st.x = 255;
		aV[2].st.y = 75;
		aV[3].st.x = 255;
		aV[3].st.y = 180;
		aV[4].st.x = 180;
		aV[4].st.y = 255;
		aV[5].st.x = 75;
		aV[5].st.y = 255;
		aV[6].st.x = 0;
		aV[6].st.y = 180;
		aV[7].st.x = 0;
		aV[7].st.y = 75;
	}

	inline void ExpandOctVertices(const SVF_Particle& RESTRICT_REFERENCE part)
	{
		ExpandOctVerticesImpl(part,aVertices.grow(8));
	}

	inline void ExpandOctVerticesImpl(const SVF_Particle& RESTRICT_REFERENCE part, SVF_Particle* aV)
	{
		CryPrefetch(aV + 8);

		const uint32* s = reinterpret_cast<const uint32*>(&part);
		uint32* a = reinterpret_cast<uint32*>(aV);
		uint32* b = reinterpret_cast<uint32*>(aV + 1);
		uint32* c = reinterpret_cast<uint32*>(aV + 2);
		uint32* d = reinterpret_cast<uint32*>(aV + 3);
		uint32* e = reinterpret_cast<uint32*>(aV + 4);
		uint32* f = reinterpret_cast<uint32*>(aV + 5);
		uint32* g = reinterpret_cast<uint32*>(aV + 6);
		uint32* h = reinterpret_cast<uint32*>(aV + 7);

		for (size_t i = 0; i < sizeof(SVF_Particle) / sizeof(uint32); ++ i)
		{
			uint32 v = s[i];
			a[i] = v;
			b[i] = v;
			c[i] = v;
			d[i] = v;
			e[i] = v;
			f[i] = v;
			g[i] = v;
			h[i] = v;
		}

		aV[0].st.x = 75;
		aV[0].st.y = 0;
		aV[1].st.x = 180;
		aV[1].st.y = 0;
		aV[2].st.x = 255;
		aV[2].st.y = 75;
		aV[3].st.x = 255;
		aV[3].st.y = 180;
		aV[4].st.x = 180;
		aV[4].st.y = 255;
		aV[5].st.x = 75;
		aV[5].st.y = 255;
		aV[6].st.x = 0;
		aV[6].st.y = 180;
		aV[7].st.x = 0;
		aV[7].st.y = 75;
	}

	inline void SetQuadIndices(int nVertAdvance = 4, bool bTessellation = false)
	{
		const uint32 nNumIndices = bTessellation? 4 : 6;
		SetQuadIndicesImpl(nVertAdvance,aIndices.grow(nNumIndices), bTessellation);
	}

  inline void SetQuadIndicesImpl(int nVertAdvance, uint16* pIndices, bool bTessellation = false )
  {
    pIndices[0] = 0 + nBaseVertexIndex;
    pIndices[1] = 1 + nBaseVertexIndex;
    pIndices[2] = 2 + nBaseVertexIndex;
    pIndices[3] = 3 + nBaseVertexIndex;

    if (!bTessellation)
    {
      pIndices[4] = 2 + nBaseVertexIndex;
      pIndices[5] = 1 + nBaseVertexIndex;
    }

    nBaseVertexIndex += nVertAdvance;
  }

	inline void SetQuadsIndices(bool bTessellation=false)
	{
		assert((aVertices.size() & 3) == 0);
		int nQuads = aVertices.size() >> 2;
		assert(aIndices.available() >= nQuads*6);
		while (nQuads-- > 0)
			SetQuadIndices(4, bTessellation);
	}

	inline void SetOctIndices()
	{
		SetOctIndicesImpl(aIndices.grow(18));
	}

	inline void SetOctIndicesImpl(uint16* pIndices)
	{
		pIndices[0] = 0 + nBaseVertexIndex;
		pIndices[1] = 1 + nBaseVertexIndex;
		pIndices[2] = 2 + nBaseVertexIndex;
		pIndices[3] = 0 + nBaseVertexIndex;
		pIndices[4] = 2 + nBaseVertexIndex;
		pIndices[5] = 4 + nBaseVertexIndex;
		pIndices[6] = 2 + nBaseVertexIndex;
		pIndices[7] = 3 + nBaseVertexIndex;
		pIndices[8] = 4 + nBaseVertexIndex;
		pIndices[9] = 0 + nBaseVertexIndex;
		pIndices[10] = 4 + nBaseVertexIndex;
		pIndices[11] = 6 + nBaseVertexIndex;
		pIndices[12] = 4 + nBaseVertexIndex;
		pIndices[13] = 5 + nBaseVertexIndex;
		pIndices[14] = 6 + nBaseVertexIndex;
		pIndices[15] = 6 + nBaseVertexIndex;
		pIndices[16] = 7 + nBaseVertexIndex;
		pIndices[17] = 0 + nBaseVertexIndex;

		nBaseVertexIndex += 8;
	}

	inline void SetOctsIndices()
	{
		assert((aVertices.size() & 7) == 0);
		int nOcts = aVertices.size() >> 3;
		assert(aIndices.available() >= nOcts*18);

		for (int i = 0; i < nOcts; i++)
		{
			SetOctIndices();
		}
	}

	inline void SetPolyIndices( int nVerts, bool bTessellation = false)
	{
		nVerts >>= 1;
		while (--nVerts > 0)
			SetQuadIndices(2, bTessellation);

		// Final quad.
		nBaseVertexIndex += 2;
	}

	void SetPoliesIndices( Array<SVF_Particle>& aSrcVerts, Array<uint16>& aSrcVertCounts )
	{
		int nAvailVerts = aVertices.available();
		int nVerts = 0;
		int nPolygon = 0;
		for (; nPolygon < aSrcVertCounts.size(); nPolygon++)
		{
			int nPolyVerts = aSrcVertCounts[nPolygon];
			if (nVerts + nPolyVerts > nAvailVerts)
				break;
			nVerts += nPolyVerts;
			SetPolyIndices(nPolyVerts);
		}
		aSrcVertCounts.erase_front(nPolygon);

		cryMemcpy(aVertices.grow_raw(nVerts), aSrcVerts.begin(), sizeof(SVF_Particle) * nVerts);
		aSrcVerts.erase_front(nVerts);
	}
};

struct IAllocRender: SRenderVertices
{
	// Render existing SVertices, alloc new ones.
	virtual bool Alloc( int nAllocVerts, int nAllocInds = 0 ) = 0; 
	virtual CREParticle* RenderElement() const { return 0; }

	virtual ~IAllocRender() {}
};

// Info computed in culling pass
struct SParticleCullInfos
{
	int nVerts;
	int nInds;
	int nCulled;
	uint16 nMaxVerticeGrow;
	uint16 nMaxVerticeShrink;
	uint16 nMaxIndiceGrow;
	uint16 nMaxIndiceShrink;
};

UNIQUE_IFACE struct IParticleVertexCreator
{
	// Create the vertices for the particle emitter.
	virtual void ComputeVertices( const CCamera& cam, IAllocRender& alloc, uint nRenderFlags, int nThreadId ) = 0;
	virtual void ComputeDrawnVertices( const CCamera& cam, uint nRenderFlags, float fMaxPixels ) = 0;
	virtual void PrepareForDrawing() = 0;
	virtual float GetDistSquared( const Vec3& vPos ) const = 0;

	virtual const SParticleCullInfos* GetCullInfos() const = 0;
	virtual ~IParticleVertexCreator() {}
};

class CREParticle : public CRendElementBase
{
public:
	CREParticle( IParticleVertexCreator* pVC, const CCamera& cam );
	void Reset( IParticleVertexCreator* pVC, const CCamera& cam );
	
	// Custom copy constructor required to avoid m_Lock copy.
	CREParticle( const CREParticle& in )
	: m_pVertexCreator(in.m_pVertexCreator)
	, m_pCamera(in.m_pCamera)
	, m_fPixels(0.f)
	{
	}
		
	virtual void GetMemoryUsage(ICrySizer *pSizer) const 
	{
	}
	// CRendElement implementation.
	static CREParticle* Create( IParticleVertexCreator* pVC, const CCamera& cam, int threadList );	
	static void PushAllOntoQueue(int threadList, CREParticle** particles, size_t numParticles);	

	static float GetMaxPixel();
	virtual CRendElementBase* mfCopyConstruct()
	{
		return new CREParticle(*this);
	}
  virtual int Size()
	{
		return sizeof(*this);
	}
	
	virtual void mfPrepare(bool bCheckOverflow);

	virtual bool mfPreDraw( SShaderPass *sl );
	virtual bool mfDraw( CShader *ef, SShaderPass *sl );

	// Additional methods.
	void PushOntoQueue(int threadList);

	void StoreVertices( const SRenderingPassInfo &passInfo, uint uRenderFlags );

	void SetVertices( Array<SVF_Particle> aVerts, Array<uint16> aVertCounts, float fPixels )
	{
		m_aVerts = aVerts;
		m_aVertCounts = aVertCounts;
		m_fPixels = fPixels;
	}

	bool operator< (const CREParticle& r) const
	{
		return false;
	}
	
	void ResetVertexCreator()
	{
		m_pVertexCreator = NULL;
	}

	void SetFirstVertexIndex( int nFirstVertexIndex )
	{
		m_nFirstVertexIndex = nFirstVertexIndex;
	}
	void SetFirstVertex( int nFirstVertex )
	{
		m_nFirstVertex = nFirstVertex;
	}

private:
	IParticleVertexCreator*							m_pVertexCreator;		// Particle object which computes vertices.
	const CCamera*											m_pCamera;					// Camera position and resolution.
	Array<SVF_Particle>									m_aVerts;						// Computed particle vertices.
	Array<uint16>												m_aVertCounts;			// Verts in each particle (multi-seg particles only).
	float																m_fPixels;					// Total pixels rendered.
	int																	m_nFirstVertexIndex;
	int																	m_nFirstVertex;
};

#endif  // __CREPARTICLE_H__
