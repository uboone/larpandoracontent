/**
 *  @file   VisualMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the visual monitoring algorithm class
 * 
 *  $Log: $
 */
#ifndef LAR_VISUAL_MONITORING_ALGORITHM_H
#define LAR_VISUAL_MONITORING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lar
{

/**
 *  @brief VisualMonitoringAlgorithm class
 */
class VisualMonitoringAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Visualize mc particle list
     * 
     *  @param  listName the list name
     */
    void VisualizeMCParticleList(const std::string &listName) const;

    /**
     *  @brief  Visualize a named vertex
     * 
     *  @param  vertexName the vertex name
     */
    void VisualizeVertex(const std::string &vertexName) const;

    /**
     *  @brief  Visualize a specified calo hit list
     * 
     *  @param  listName the list name
     */
    void VisualizeCaloHitList(const std::string &listName) const;

    /**
     *  @brief  Visualize a specified calo hit list
     * 
     *  @param  listName the list name
     */
    void VisualizeClusterList(const std::string &listName) const;

    /**
     *  @brief  Visualize a specified calo hit list
     * 
     *  @param  listName the list name
     */
    void VisualizeParticleFlowList(const std::string &listName) const;

    typedef std::map<int, float> PdgCodeToEnergyMap;

    bool                    m_showCurrentMCParticles;   ///< Whether to show current mc particles
    pandora::StringVector   m_mcParticleListNames;      ///< Names of mc particles lists to show

    bool                    m_showCurrentVertex;        ///< Whether to show current vertex
    pandora::StringVector   m_vertexNames;              ///< Names of the vertices to show

    bool                    m_showCurrentCaloHits;      ///< Whether to show current calohitlist
    pandora::StringVector   m_caloHitListNames;         ///< Names of calo hit lists to show

    bool                    m_showCurrentClusters;      ///< Whether to show current clusters
    pandora::StringVector   m_clusterListNames;         ///< Names of cluster lists to show

    bool                    m_showCurrentPfos;          ///< Whether to show current particle flow object list
    pandora::StringVector   m_pfoListNames;             ///< Names of pfo lists to show

    bool                    m_showOnlyAvailable;        ///< Whether to show only available  (i.e. non-clustered) calohits and tracks
    bool                    m_displayEvent;             ///< Whether to display the event

    bool                    m_blackBackground;          ///< Whether to use a black background color, rather than white
    bool                    m_showDetector;             ///< Whether to display the detector geometry
    std::string             m_hitColors;                ///< Define the hit coloring scheme (default: pfo, choices: pfo, particleid)

    float                   m_thresholdEnergy;          ///< Cell energy threshold for display (em scale)
    float                   m_transparencyThresholdE;   ///< Cell energy for which transparency is saturated (0%, fully opaque)
    float                   m_energyScaleThresholdE;    ///< Cell energy for which color is at top end of continous color palette

    pandora::StringVector   m_suppressMCParticles;      ///< List of PDG numbers and energies for MC particles to be suppressed (e.g. " 22:0.1 2112:1.0 ")
    PdgCodeToEnergyMap      m_particleSuppressionMap;   ///< Map from pdg-codes to energy for suppression of particles types below specific energies
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *VisualMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new VisualMonitoringAlgorithm();
}

} // namespace lar

#endif // #ifndef LAR_VISUAL_MONITORING_ALGORITHM_H
