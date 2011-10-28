#include <iostream>
#include <limits>

#include "protein/residue_id.h"

#include "contacto/residue_contact_area_difference_profile.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/map_io.h"

typedef std::map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore> Profile;

const double GAP_VALUE=-1;
const double NO_VALUE=-2;

std::map<protein::ResidueID, double> construct_local_scores_from_profile(const Profile& profile, const std::string& category)
{
	std::map<protein::ResidueID, double> local_scores;
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

void main_calculate_contact_area_difference_local_scores(const auxiliaries::CommandLineOptions& clo)
{
	const std::string category=clo.arg<std::string>("--category") ;

	auxiliaries::assert_file_header("cad_profile");
	const Profile profile=auxiliaries::read_map<protein::ResidueID, contacto::ResidueContactAreaDifferenceScore>();

	const std::map<protein::ResidueID, double> local_scores=construct_local_scores_from_profile(profile, category);

	auxiliaries::print_file_header("local_scores");
	for(std::map<protein::ResidueID, double>::const_iterator it=local_scores.begin();it!=local_scores.end();++it)
	{
		std::cout << it->first << " " << it->second << "\n";
	}
}
