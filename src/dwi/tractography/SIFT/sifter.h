/*
    Copyright 2011 Brain Research Institute, Melbourne, Australia

    Written by Robert Smith, 2011.

    This file is part of MRtrix.

    MRtrix is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MRtrix is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

 */



#ifndef __dwi_tractography_sift_sifter_h__
#define __dwi_tractography_sift_sifter_h__


#include <vector>


#include "dwi/fixel_map.h"

#include "dwi/directions/set.h"

#include "dwi/tractography/SIFT/fixel.h"
#include "dwi/tractography/SIFT/gradient_sort.h"
#include "dwi/tractography/SIFT/model.h"
#include "dwi/tractography/SIFT/output.h"
#include "dwi/tractography/SIFT/track_index_range.h"
#include "dwi/tractography/SIFT/types.h"

#include "image/buffer.h"
#include "image/header.h"



namespace MR
{
  namespace DWI
  {
    namespace Tractography
    {
      namespace SIFT
      {


      class SIFTer : public Model<Fixel>
      {

        protected:
        typedef Model<Fixel> MapType;
        typedef Fixel_map<Fixel>::MapVoxel MapVoxel;
        typedef Fixel_map<Fixel>::VoxelAccessor VoxelAccessor;


        public:
        SIFTer (Image::Buffer<float>& i, const DWI::Directions::FastLookupSet& d) :
            MapType (i, d),
            output_debug (false),
            term_number (0),
            term_ratio (0.0),
            term_mu (0.0),
            enforce_quantisation (true) { }

        ~SIFTer() { }


        // CORE OPERATIONS
        void perform_filtering();
        void output_filtered_tracks (const std::string&, const std::string&) const;
        void output_selection (const std::string&) const;

        // CONFIGURATION OPTIONS
        void set_term_number (const track_t i)      { term_number = i; }
        void set_term_ratio  (const float i)        { term_ratio = i; }
        void set_term_mu     (const float i)        { term_mu = i; }
        void set_csv_path    (const std::string& i) { csv_path = i; }

        void set_regular_outputs (const std::vector<int>&, const bool);


        // DEBUGGING
        void test_sorting_block_size (const size_t) const;


        protected:
        using Fixel_map<Fixel>::accessor;
        using Fixel_map<Fixel>::fixels;

        using MapType::H;
        using MapType::FOD_sum;
        using MapType::TD_sum;
        using MapType::mu;
        using MapType::proc_mask;
        using MapType::num_tracks;


        // User-controllable settings
        std::vector<track_t> output_at_counts;
        bool    output_debug;
        track_t term_number;
        float   term_ratio;
        double  term_mu;
        bool    enforce_quantisation;
        std::string csv_path;


        // Convenience functions
        double calc_roc_cost_function() const;
        double calc_gradient (const track_t, const double, const double) const;



        // For calculating the streamline removal gradients in a multi-threaded fashion
        class TrackGradientCalculator
        {
          public:
            TrackGradientCalculator (const SIFTer& sifter, std::vector<Cost_fn_gradient_sort>& v, const double mu, const double r) :
              master (sifter), gradient_vector (v), current_mu (mu), current_roc_cost (r) { }
            bool operator() (const TrackIndexRange&) const;
          private:
            const SIFTer& master;
            std::vector<Cost_fn_gradient_sort>& gradient_vector;
            const double current_mu, current_roc_cost;
        };



        SIFTer (const SIFTer& that) :
            MapType (that),
            output_debug (false),
            term_number (0),
            term_ratio (0.0),
            term_mu (0.0),
            enforce_quantisation (true) { assert (0); }


      };




      }
    }
  }
}


#endif


