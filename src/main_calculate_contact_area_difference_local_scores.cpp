#include <iostream>
#include <limits>

#include "protein/residue_id.h"

#include "contacto/residue_contact_area_difference_profile.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/map_io.h"

typedef std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> Profile;
typedef std::map<protein::ResidueID, double> LocalScores;

const double GAP_VALUE=-1;
const double NO_VALUE=-2;

LocalScores construct_local_scores_from_profile(const Profile& profile, const std::string& category)
{
	LocalScores local_scores;
	for(Profile::const_iterator it=profile.begin();it!=profile.end();++it)
	{
		const protein::ResidueID& residue_id=it->first;
		const contacto::ResidueContactAreaDifferenceScore& residue_score=it->second;

		if(it!=profile.begin())
		{
			Profile::const_iterator prev_it=it;
			prev_it--;
			const protein::ResidueID& prev_residue_id=prev_it->first;
			if((residue_id.residue_number-prev_residue_id.residue_number>1) && residue_id.chain_id==prev_residue_id.chain_id)
			{
				for(int i=prev_residue_id.residue_number+1;i<residue_id.residue_number;i++)
				{
					local_scores[protein::ResidueID(prev_residue_id.chain_id, i)]=GAP_VALUE;
				}
			}
		}

		contacto::ResidueContactAreaDifferenceScore::Ratio ratio=residue_score.ratio(category);
		local_scores[residue_id]=(ratio.reference>0) ? (ratio.difference/ratio.reference) : NO_VALUE;
	}
	return local_scores;
}

std::map<protein::ResidueID, double> blur_local_scores(const LocalScores& local_scores, const int window_size)
{
	if(window_size==0)
	{
		return local_scores;
	}

	LocalScores blured_scores;
	LocalScores::iterator prev=blured_scores.begin();
	for(LocalScores::const_iterator it=local_scores.begin();it!=local_scores.end();++it)
	{
		const protein::ResidueID& residue_id=it->first;
		double sum=it->second;
		int count=1;
		if(it->second>0)
		{
			LocalScores::const_iterator left=it;
			LocalScores::const_iterator right=it;
			for(int i=1;i<=window_size;i++)
			{
				if(left!=local_scores.begin())
				{
					left--;
					const protein::ResidueID& left_residue_id=left->first;
					if(residue_id.residue_number-left_residue_id.residue_number<=window_size && residue_id.chain_id==left_residue_id.chain_id)
					{
						if(left->second>0)
						{
							sum+=left->second;
							count++;
						}
					}
				}
				if(right!=local_scores.end())
				{
					right++;
					const protein::ResidueID& right_residue_id=right->first;
					if(right_residue_id.residue_number-residue_id.residue_number<=window_size && residue_id.chain_id==right_residue_id.chain_id)
					{
						if(right->second>0)
						{
							sum+=right->second;
							count++;
						}
					}
				}
			}
		}
		prev=blured_scores.insert(prev, std::make_pair(residue_id, (sum/count)));
	}
	return blured_scores;
}

void main_calculate_contact_area_difference_local_scores(const auxiliaries::CommandLineOptions& clo)
{
	const std::string category=clo.arg<std::string>("--category");
	const int window_size=clo.arg<int>("--window", 0, 1000);

	auxiliaries::assert_file_header("cad_profile");
	const Profile profile=auxiliaries::read_map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore>();

	const LocalScores local_scores=blur_local_scores(construct_local_scores_from_profile(profile, category), window_size);

	auxiliaries::print_file_header("local_scores");
	auxiliaries::print_map(local_scores, false);
}
