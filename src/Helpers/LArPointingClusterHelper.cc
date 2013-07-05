/**
 *  @file   LArContent/src/Helpers/LArPointingClusterHelper.cc
 * 
 *  @brief  Implementation of the pointing cluster helper class.
 * 
 *  $Log: $
 */

#include "Helpers/ClusterHelper.h"
#include "Helpers/XmlHelper.h"

#include "Helpers/LArPointingClusterHelper.h"
#include "Helpers/LArVertexHelper.h"

using namespace pandora;

namespace lar
{

//------------------------------------------------------------------------------------------------------------------------------------------

bool LArPointingClusterHelper::IsNode(const CartesianVector &parentVertex, const CartesianVector &daughterVertex)
{
    if ((parentVertex - daughterVertex).GetMagnitudeSquared() < m_maxNodeRadiusSquared)
        return true;

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LArPointingClusterHelper::IsEmission(const LArPointingCluster::Vertex &parentPointingVertex, const pandora::CartesianVector &daughterVertex)
{
    return LArPointingClusterHelper::IsPointing(daughterVertex, parentPointingVertex);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LArPointingClusterHelper::IsEmitted(const pandora::CartesianVector &parentVertex, const LArPointingCluster::Vertex &daughterPointingVertex)
{
    return LArPointingClusterHelper::IsPointing(parentVertex, daughterPointingVertex);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LArPointingClusterHelper::IsPointing(const pandora::CartesianVector &vertex, const LArPointingCluster::Vertex &pointingVertex)
{
    const CartesianVector displacement(pointingVertex.GetPosition() - vertex);

    const float longitudinalDistance(pointingVertex.GetDirection().GetDotProduct(displacement));

    if ((longitudinalDistance < m_minPointingLongitudinalDistance) || (longitudinalDistance > m_maxPointingLongitudinalDistance))
        return false;

    const float maxTransverseDistanceSquared((m_maxPointingTransverseDistance * m_maxPointingTransverseDistance) + 
        (m_pointingAngularAllowance * m_pointingAngularAllowance * longitudinalDistance * longitudinalDistance));

    const float transverseDistanceSquared(pointingVertex.GetDirection().GetCrossProduct(displacement).GetMagnitudeSquared());

    if (transverseDistanceSquared > maxTransverseDistanceSquared)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LArPointingClusterHelper::GetAverageDirection( const LArPointingCluster::Vertex& firstVertex, const LArPointingCluster::Vertex& secondVertex, CartesianVector& averageDirection )
{
    const Cluster* pFirstCluster = firstVertex.GetCluster();
    const Cluster* pSecondCluster = secondVertex.GetCluster();

    if ( pFirstCluster==pSecondCluster )
        throw pandora::StatusCodeException(STATUS_CODE_NOT_ALLOWED);

    const float E1 = pFirstCluster->GetHadronicEnergy();
    const float E2 = pSecondCluster->GetHadronicEnergy();

    if ( E1 < std::numeric_limits<float>::epsilon() || E2 < std::numeric_limits<float>::epsilon() )
        throw pandora::StatusCodeException(STATUS_CODE_NOT_ALLOWED);

    // calculate average direction
    averageDirection = ( firstVertex.GetDirection() * E1 + secondVertex.GetDirection() * E2 ).GetUnitVector();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LArPointingClusterHelper::GetIntersection( const LArPointingCluster::Vertex& firstVertex, const LArPointingCluster::Vertex& secondVertex, CartesianVector& intersectPosition, float& firstDisplacement, float& secondDisplacement )
{ 
    const Cluster* pFirstCluster = firstVertex.GetCluster();
    const Cluster* pSecondCluster = secondVertex.GetCluster();

    if ( pFirstCluster==pSecondCluster )
        throw pandora::StatusCodeException(STATUS_CODE_NOT_ALLOWED);

    LArPointingClusterHelper::GetIntersection( firstVertex.GetPosition(), firstVertex.GetDirection(), 
        secondVertex.GetPosition(), secondVertex.GetDirection(), intersectPosition, 
        firstDisplacement, secondDisplacement );
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LArPointingClusterHelper::GetIntersection( const CartesianVector& a1, const CartesianVector& a2, const CartesianVector& b1, const CartesianVector& b2, CartesianVector& intersectPosition, float& firstDisplacement, float& secondDisplacement )
{  
    // note: input lines are r = a1 + P * a2 and r = b1 + Q * b2

    // relative angle between direction vector
    const float cosTheta = a2.GetDotProduct(b2);

    // lines must be non-parallel
    if ( 1.f - std::fabs(cosTheta) < std::numeric_limits<float>::epsilon() )
        throw pandora::StatusCodeException(STATUS_CODE_NOT_ALLOWED);

    // calculate the intersection (by minimising the distance between the lines)
    const float P = ( (a2 - b2 * cosTheta).GetDotProduct(b1-a1) ) / (1.0 - cosTheta*cosTheta);
    const float Q = ( (a2 * cosTheta - b2).GetDotProduct(b1-a1) ) / (1.0 - cosTheta*cosTheta);

    // position of intersection (or point of closest approach)
    intersectPosition = ( a1 + a2 * P + b1 + b2 * Q ) * 0.5;

    // displacements of intersection from input vertices
    firstDisplacement = P;   secondDisplacement = Q;

    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LArPointingClusterHelper::GetIntersection( const LArPointingCluster::Vertex& vertexCluster, const Cluster *const pTargetCluster, CartesianVector& intersectPosition, float& displacementL, float& displacementT )
{       
    displacementT = +std::numeric_limits<float>::max();
    displacementL = -std::numeric_limits<float>::max();

    float rL(0.f), rT(0.f);
    float figureOfMerit(std::numeric_limits<float>::max());
    float foundIntersection(false);

    const OrderedCaloHitList &orderedCaloHitList(pTargetCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter1 = orderedCaloHitList.begin(), iterEnd1 = orderedCaloHitList.end(); iter1 != iterEnd1; ++iter1)
    {
        for (CaloHitList::const_iterator iter2 = iter1->second->begin(), iterEnd2 = iter1->second->end(); iter2 != iterEnd2; ++iter2)
	{
	    const CartesianVector& hitPosition = (*iter2)->GetPositionVector();

            LArVertexHelper::GetImpactParameters( vertexCluster.GetPosition(), vertexCluster.GetDirection(), hitPosition, rL, rT );

            if (rT < figureOfMerit)
	    {
	        figureOfMerit = rT;
            
                displacementL = rL;  
                displacementT = rT;
                intersectPosition = hitPosition; 
                foundIntersection = true;
	    }
	}
    }

    if (false == foundIntersection)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float LArPointingClusterHelper::m_maxNodeRadiusSquared = 2.5f * 2.5f;
float LArPointingClusterHelper::m_maxPointingLongitudinalDistance = 20.f;
float LArPointingClusterHelper::m_minPointingLongitudinalDistance = -2.5f;
float LArPointingClusterHelper::m_maxPointingTransverseDistance = 2.5f;
float LArPointingClusterHelper::m_pointingAngularAllowance = 0.0175f; // tan (1 degree)

StatusCode LArPointingClusterHelper::ReadSettings(const TiXmlHandle xmlHandle)
{
    float maxNodeRadius = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, 
        "MaxNodeRadius", maxNodeRadius));
    m_maxNodeRadiusSquared = maxNodeRadius * maxNodeRadius;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, 
        "MaxPointingLongitudinalDistance", m_maxPointingLongitudinalDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, 
        "MinPointingLongitudinalDistance", m_minPointingLongitudinalDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, 
        "MaxPointingTransverseDistance", m_maxPointingTransverseDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, 
        "PointingAngularAllowance", m_pointingAngularAllowance));

    return STATUS_CODE_SUCCESS;
}

} // namespace lar
