#include <iostream>
#include <memory>

#include "protein/atom.h"
#include "protein/residue_id.h"
#include "protein/residue_ids_intervals.h"

#include "contacto/inter_residue_contact_id.h"
#include "contacto/inter_residue_contact_dual_areas.h"
#include "contacto/contact_classification.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"
#include "apollo/hyperbolic_cell_face.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"
#include "auxiliaries/name_colorizing.h"
#include "auxiliaries/opengl_printer.h"

class ResidueNameColorizerByResidueType : public auxiliaries::NameColorizerForPymol<std::string>
{
public:
	ResidueNameColorizerByResidueType()
	{
		set_map_of_colors(create_map_of_residue_colors_by_type());
	}

private:
	static std::map<std::string, auxiliaries::Color> create_map_of_residue_colors_by_type()
	{
		const auxiliaries::Color nonpolar(255, 255, 0);
		const auxiliaries::Color acidic(255, 0, 0);
		const auxiliaries::Color basic(0, 0, 255);
		const auxiliaries::Color uncharged(0, 255, 0);

		std::map<std::string, auxiliaries::Color> m;

		m["LEU"]=nonpolar;
		m["VAL"]=nonpolar;
		m["ILE"]=nonpolar;
		m["ALA"]=nonpolar;
		m["PHE"]=nonpolar;
		m["TRP"]=nonpolar;
		m["MET"]=nonpolar;
		m["PRO"]=nonpolar;

		m["ASP"]=acidic;
		m["GLU"]=acidic;

		m["LYS"]=basic;
		m["ARG"]=basic;
		m["HIS"]=basic;

		m["CYS"]=uncharged;
		m["SER"]=uncharged;
		m["THR"]=uncharged;
		m["TYR"]=uncharged;
		m["ASN"]=uncharged;
		m["GLN"]=uncharged;
		m["GLY"]=uncharged;

		return m;
	}
};

class ResidueNameColorizerByResidueHydrophobicity : public auxiliaries::NameColorizerForPymol<std::string>
{
public:
	ResidueNameColorizerByResidueHydrophobicity()
	{
		set_map_of_colors(create_map_of_residue_colors_by_hydropathy_indices());
	}

private:
	static auxiliaries::Color color_from_hydropathy_index(const double hi)
	{
		return auxiliaries::Color::from_temperature_to_blue_white_red((1+hi/4.5)/2);
	}

	static std::map<std::string, auxiliaries::Color> create_map_of_residue_colors_by_hydropathy_indices()
	{
		std::map<std::string, auxiliaries::Color> m;
		m["ASP"]=color_from_hydropathy_index(-3.5);
		m["GLU"]=color_from_hydropathy_index(-3.5);
		m["CYS"]=color_from_hydropathy_index(2.5);
		m["MET"]=color_from_hydropathy_index(1.9);
		m["LYS"]=color_from_hydropathy_index(-3.9);
		m["ARG"]=color_from_hydropathy_index(-4.5);
		m["SER"]=color_from_hydropathy_index(-0.8);
		m["THR"]=color_from_hydropathy_index(-0.7);
		m["PHE"]=color_from_hydropathy_index(2.8);
		m["TYR"]=color_from_hydropathy_index(-1.3);
		m["ASN"]=color_from_hydropathy_index(-3.5);
		m["GLN"]=color_from_hydropathy_index(-3.5);
		m["GLY"]=color_from_hydropathy_index(-0.4);
		m["LEU"]=color_from_hydropathy_index(3.8);
		m["VAL"]=color_from_hydropathy_index(4.2);
		m["ILE"]=color_from_hydropathy_index(4.5);
		m["ALA"]=color_from_hydropathy_index(1.8);
		m["TRP"]=color_from_hydropathy_index(-0.9);
		m["HIS"]=color_from_hydropathy_index(-3.2);
		m["PRO"]=color_from_hydropathy_index(-1.6);
		return m;
	}
};

class AtomNameColorizerByAtomType : public auxiliaries::NameColorizerForPymol<std::string>
{
public:
	AtomNameColorizerByAtomType()
	{
		set_map_of_colors(create_map_of_atom_colors_by_type());
	}

private:
	static std::map<std::string, auxiliaries::Color> create_map_of_atom_colors_by_type()
	{
		std::map<std::string, auxiliaries::Color> m;

		m["C"]=auxiliaries::Color(0, 255, 0);
		m["N"]=auxiliaries::Color(0, 0, 255);
		m["O"]=auxiliaries::Color(255, 0, 0);
		m["S"]=auxiliaries::Color(255, 255, 0);
		m["P"]=auxiliaries::Color(255, 0, 255);

		return m;
	}
};

class ValueColorizer : public auxiliaries::NameColorizerForPymol<int>
{
public:
	ValueColorizer()
	{
		set_map_of_colors(create_map_of_colors_by_values());
	}

private:
	static std::map<int, auxiliaries::Color> create_map_of_colors_by_values()
	{
		std::map<int, auxiliaries::Color> m;
		for(int i=0;i<=100;i++)
		{
			m[i]=auxiliaries::Color::from_temperature_to_blue_white_red(static_cast<double>(i)/100.0);
		}
		m[-1]=auxiliaries::Color(255, 0, 255);
		return m;
	}
};

class ValuePairColorizer : public auxiliaries::NameColorizerForPymol< std::pair<int, int> >
{
public:
	ValuePairColorizer()
	{
		set_map_of_colors(create_map_of_colors_by_value_pairs());
	}

	static const int MAX_PAIR_MEMBER=20;

private:
	static std::map<std::pair<int, int>, auxiliaries::Color> create_map_of_colors_by_value_pairs()
	{
		std::map<std::pair<int, int>, auxiliaries::Color> m;
		for(int i=0;i<=MAX_PAIR_MEMBER;i++)
		{
			for(int j=0;j<=MAX_PAIR_MEMBER;j++)
			{
				m[std::make_pair(i, j)]=auxiliaries::Color::from_two_values_to_green_yellow_red(i, j);
			}
		}
		m[std::make_pair(-1, -1)]=auxiliaries::Color(255, 0, 255);
		return m;
	}
};

class ContactColorizerInterface
{
public:
	virtual auxiliaries::Color color(const protein::Atom& a, const protein::Atom& b) const = 0;

	std::string color_string(const protein::Atom& a, const protein::Atom& b) const
	{
		return auxiliaries::ColorManagementForPymol::color_to_string_id(color(a, b));
	}

	virtual void list_colors() const = 0;
};

template<class ResidueNameColorizerType>
class ContactColorizerByFirstResidueName : public ContactColorizerInterface
{
public:
	ContactColorizerByFirstResidueName()
	{
	}

	auxiliaries::Color color(const protein::Atom& a, const protein::Atom& b) const
	{
		return name_colorizer_.color(a.residue_name);
	}

	virtual void list_colors() const
	{
		name_colorizer_.list_colors();
	}

private:
	ResidueNameColorizerType name_colorizer_;
};

class ContactColorizerByFirstAtomName : public ContactColorizerInterface
{
public:
	ContactColorizerByFirstAtomName()
	{
	}

	auxiliaries::Color color(const protein::Atom& a, const protein::Atom& b) const
	{
		return name_colorizer_.color(a.atom_name.substr(0, 1));
	}

	virtual void list_colors() const
	{
		name_colorizer_.list_colors();
	}

private:
	AtomNameColorizerByAtomType name_colorizer_;
};

class ContactColorizerByInterResidueContactScore : public ContactColorizerInterface
{
public:
	ContactColorizerByInterResidueContactScore(
			const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >& combined_inter_residue_contacts,
			const bool use_only_all_to_all_scores) :
				combined_inter_residue_contacts_(combined_inter_residue_contacts),
				use_only_all_to_all_scores_(use_only_all_to_all_scores)
	{
	}

	auxiliaries::Color color(const protein::Atom& a, const protein::Atom& b) const
	{
		const contacto::InterResidueContactID<protein::ResidueID> irc_id(protein::ResidueID::from_atom(a), protein::ResidueID::from_atom(b));
		std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >::const_iterator it=combined_inter_residue_contacts_.find(irc_id);
		if(it!=combined_inter_residue_contacts_.end())
		{
			const std::string contact_type=use_only_all_to_all_scores_ ? std::string("AA") : contacto::ContactClassification::classify_atoms_contact<protein::Atom, protein::ResidueID>(a, b).front();
			std::pair<double, double> area=it->second.area(contact_type);
			if(area.first>0.0)
			{
				return name_colorizer_.color(static_cast<int>(floor((std::min(fabs(area.first-area.second), area.first)/area.first)*100.0)));
			}
		}
		return name_colorizer_.color(-1);
	}

	virtual void list_colors() const
	{
		name_colorizer_.list_colors();
	}

private:
	std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts_;
	bool use_only_all_to_all_scores_;
	ValueColorizer name_colorizer_;
};

class ContactColorizerByInterResidueContactAreaPair : public ContactColorizerInterface
{
public:
	ContactColorizerByInterResidueContactAreaPair(
			const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >& combined_inter_residue_contacts,
			const bool use_only_all_to_all_scores) :
				combined_inter_residue_contacts_(combined_inter_residue_contacts),
				use_only_all_to_all_scores_(use_only_all_to_all_scores)
	{
	}

	auxiliaries::Color color(const protein::Atom& a, const protein::Atom& b) const
	{
		const contacto::InterResidueContactID<protein::ResidueID> irc_id(protein::ResidueID::from_atom(a), protein::ResidueID::from_atom(b));
		std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >::const_iterator it=combined_inter_residue_contacts_.find(irc_id);
		if(it!=combined_inter_residue_contacts_.end())
		{
			const std::string contact_type=use_only_all_to_all_scores_ ? std::string("AA") : contacto::ContactClassification::classify_atoms_contact<protein::Atom, protein::ResidueID>(a, b).front();
			std::pair<double, double> area=it->second.area(contact_type);
			const double max_area=std::max(area.first, area.second);
			if(max_area>0)
			{
				return name_colorizer_.color(std::make_pair(static_cast<int>(area.first/max_area*static_cast<double>(name_colorizer_.MAX_PAIR_MEMBER)), static_cast<int>(area.second/max_area*static_cast<double>(name_colorizer_.MAX_PAIR_MEMBER))));
			}
		}
		return name_colorizer_.color(std::make_pair(-1, -1));
	}

	virtual void list_colors() const
	{
		name_colorizer_.list_colors();
	}

private:
	std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts_;
	bool use_only_all_to_all_scores_;
	ValuePairColorizer name_colorizer_;
};

class ContactColorizerByFirstResidueID : public ContactColorizerInterface
{
public:
	ContactColorizerByFirstResidueID(const std::vector<protein::Atom>& atoms)
	{
		for(std::size_t i=0;i<atoms.size();i++)
		{
			const long numeric_rid=numeric_residue_id(protein::ResidueID::from_atom(atoms[i]));
			name_colorizer_.add_name_color(numeric_rid, auxiliaries::Color::from_id(numeric_rid));
		}
	}

	auxiliaries::Color color(const protein::Atom& a, const protein::Atom& b) const
	{
		return name_colorizer_.color(numeric_residue_id(protein::ResidueID::from_atom(a)));
	}

	virtual void list_colors() const
	{
		name_colorizer_.list_colors();
	}

private:
	static long numeric_residue_id(const protein::ResidueID& residue_id)
	{
		return static_cast<long>(residue_id.residue_number)+(residue_id.chain_id.empty() ? 0 : static_cast<long>(residue_id.chain_id.c_str()[0])*1000000);
	}

	auxiliaries::NameColorizerForPymol<long> name_colorizer_;
};

class ContactColorizerByFirstAtomID : public ContactColorizerInterface
{
public:
	ContactColorizerByFirstAtomID(const std::vector<protein::Atom>& atoms)
	{
		for(std::size_t i=0;i<atoms.size();i++)
		{
			const protein::Atom& a=atoms[i];
			name_colorizer_.add_name_color(a.atom_number, auxiliaries::Color::from_id(a.atom_number));
		}
	}

	auxiliaries::Color color(const protein::Atom& a, const protein::Atom& b) const
	{
		return name_colorizer_.color(a.atom_number);
	}

	virtual void list_colors() const
	{
		name_colorizer_.list_colors();
	}

private:
	auxiliaries::NameColorizerForPymol<int> name_colorizer_;
};

class ContactAccepterInterface
{
public:
	virtual bool accept(const protein::Atom& a, const protein::Atom& b) const = 0;
	virtual std::string assign_group_name(const protein::Atom& a) const = 0;
};

class ContactAccepterForInterChain : public ContactAccepterInterface
{
public:
	bool accept(const protein::Atom& a, const protein::Atom& b) const
	{
		return (a.chain_id!=b.chain_id && a.chain_id!="?" && b.chain_id!="?");
	}

	std::string assign_group_name(const protein::Atom& a) const
	{
		return a.chain_id;
	}
};

class ContactAccepterForInterResidue : public ContactAccepterInterface
{
public:
	bool accept(const protein::Atom& a, const protein::Atom& b) const
	{
		return (protein::ResidueID::from_atom(a)!=protein::ResidueID::from_atom(b));
	}

	std::string assign_group_name(const protein::Atom& a) const
	{
		std::ostringstream output;
		output << "r" << a.residue_name;
		return output.str();
	}
};

class ContactAccepterForInterInterval : public ContactAccepterInterface
{
public:
	ContactAccepterForInterInterval(std::vector< std::vector< std::pair<protein::ResidueID, protein::ResidueID> > > intervals) : intervals_(intervals)
	{
	}

	bool accept(const protein::Atom& a, const protein::Atom& b) const
	{
		const int a_iid=interval_id(a);
		const int b_iid=interval_id(b);
		if(a_iid!=b_iid)
		{
			if(a_iid>=0 && b_iid>=0)
			{
				return true;
			}
			else if(intervals_.size()==1)
			{
				return true;
			}
		}
		return false;
	}

	std::string assign_group_name(const protein::Atom& a) const
	{
		const int iid=interval_id(a);
		if(iid>=0)
		{
			std::ostringstream output;
			output << iid;
			return output.str();
		}
		else
		{
			return "all";
		}
	}

private:
	int interval_id(const protein::Atom& a) const
	{
		for(std::size_t i=0;i<intervals_.size();i++)
		{
			for(std::size_t j=0;j<intervals_[i].size();j++)
			{
				const protein::ResidueID& r1=intervals_[i][j].first;
				const protein::ResidueID& r2=intervals_[i][j].second;
				if(a.chain_id==r1.chain_id && a.residue_number>=r1.residue_number && a.residue_number<=r2.residue_number)
				{
					return i;
				}
			}
		}
		return (-1);
	}

	std::vector< std::vector< std::pair<protein::ResidueID, protein::ResidueID> > > intervals_;
};

void print_inter_chain_interface_graphics(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;
	typedef apollo::HyperbolicCellFace CellFace;

	clo.check_allowed_options("--probe: --step: --projections: --face-coloring: --selection-coloring: --groups: --output-names-prefix:");

	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;
	const double step_length=clo.isopt("--step") ? clo.arg_with_min_value<double>("--step", 0.1) : 0.5;
	const int projections_count=clo.isopt("--projections") ? clo.arg_with_min_value<int>("--projections", 5) : 5;
	const std::string face_coloring_mode=clo.isopt("--face-coloring") ? clo.arg<std::string>("--face-coloring") : std::string("");
	const std::string selection_coloring_mode=clo.isopt("--selection-coloring") ? clo.arg<std::string>("--selection-coloring") : std::string("");
	const std::string groups_option=clo.isopt("--groups") ? clo.arg<std::string>("--groups") : std::string("");
	const std::string output_names_prefix=clo.isopt("--output-names-prefix") ? clo.arg<std::string>("--output-names-prefix") : std::string("");

	auxiliaries::assert_file_header(std::cin, "atoms");
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);

	std::auto_ptr<ContactAccepterInterface> contact_accepter;
	if(groups_option.substr(0,1)=="(")
	{
		std::vector< std::vector< std::pair<protein::ResidueID, protein::ResidueID> > > intervals;
		if(!protein::ResidueIDsIntervalsReader::read_residue_ids_intervals(groups_option, intervals) || intervals.empty())
		{
			throw std::runtime_error(std::string("Invalid intervals string: ")+groups_option);
		}
		contact_accepter.reset(new ContactAccepterForInterInterval(intervals));
	}
	else if(groups_option=="inter_residue")
	{
		contact_accepter.reset(new ContactAccepterForInterResidue());
	}
	else
	{
		contact_accepter.reset(new ContactAccepterForInterChain());
	}

	std::auto_ptr<const ContactColorizerInterface> face_colorizer;
	if(face_coloring_mode=="residue_type")
	{
		face_colorizer.reset(new ContactColorizerByFirstResidueName<ResidueNameColorizerByResidueType>());
	}
	else if(face_coloring_mode=="residue_hydrophobicity")
	{
		face_colorizer.reset(new ContactColorizerByFirstResidueName<ResidueNameColorizerByResidueHydrophobicity>());
	}
	else if(face_coloring_mode=="atom_type")
	{
		face_colorizer.reset(new ContactColorizerByFirstAtomName());
	}
	else if(face_coloring_mode=="inter_residue_contact_scores" || face_coloring_mode=="inter_residue_contact_AA_scores")
	{
		auxiliaries::assert_file_header(std::cin, "combined_residue_contacts");
		const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=
				auxiliaries::read_map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >(std::cin);
		face_colorizer.reset(new ContactColorizerByInterResidueContactScore(combined_inter_residue_contacts, face_coloring_mode=="inter_residue_contact_AA_scores"));
	}
	else if(face_coloring_mode=="inter_residue_contact_area_pairs" || face_coloring_mode=="inter_residue_contact_AA_area_pairs")
	{
		auxiliaries::assert_file_header(std::cin, "combined_residue_contacts");
		const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > combined_inter_residue_contacts=
				auxiliaries::read_map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas >(std::cin);
		face_colorizer.reset(new ContactColorizerByInterResidueContactAreaPair(combined_inter_residue_contacts, face_coloring_mode=="inter_residue_contact_AA_area_pairs"));
	}
	else if(face_coloring_mode=="residue_id")
	{
		face_colorizer.reset(new ContactColorizerByFirstResidueID(atoms));
	}
	else if(face_coloring_mode=="atom_id")
	{
		face_colorizer.reset(new ContactColorizerByFirstAtomID(atoms));
	}
	else
	{
		face_colorizer.reset(new ContactColorizerByFirstResidueName< auxiliaries::NameColorizerForPymol<std::string> >());
	}

	const Hierarchy hierarchy(atoms, 4.2, 1);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, true);
	const Apollo::PairsNeighboursMap pairs_neighbours_map=Apollo::collect_pairs_neighbours_from_quadruples(quadruples_map);

	std::vector<CellFace> faces_vector;
	std::map< std::pair<std::size_t, std::size_t>, std::size_t > faces_vector_map;
	typedef std::map< std::pair<std::string, std::string>, std::vector< std::pair<std::size_t, std::size_t> > > InterfacesMap;
	InterfacesMap inter_chain_interfaces;

	for(Apollo::PairsNeighboursMap::const_iterator it=pairs_neighbours_map.begin();it!=pairs_neighbours_map.end();++it)
	{
		const std::pair<std::size_t, std::size_t> atoms_ids_pair=std::make_pair(it->first.get(0), it->first.get(1));

		const protein::Atom& a=atoms[atoms_ids_pair.first];
		const protein::Atom& b=atoms[atoms_ids_pair.second];

		if(contact_accepter->accept(a,b))
		{
			std::vector<const protein::Atom*> cs;
			cs.reserve(it->second.size());
			for(Apollo::PairsNeighboursMap::mapped_type::const_iterator jt=it->second.begin();jt!=it->second.end();++jt)
			{
				cs.push_back(&(atoms[*jt]));
			}

			const CellFace cell_face=CellFace::construct(a, b, cs, probe_radius, step_length, projections_count);
			if(!cell_face.mesh_vertices().empty())
			{
				const std::pair<std::size_t, std::size_t> reversed_atoms_ids_pair=std::make_pair(atoms_ids_pair.second, atoms_ids_pair.first);
				faces_vector.push_back(cell_face);
				faces_vector_map[atoms_ids_pair]=faces_vector.size()-1;
				faces_vector_map[reversed_atoms_ids_pair]=faces_vector.size()-1;
				inter_chain_interfaces[std::make_pair(contact_accepter->assign_group_name(a), contact_accepter->assign_group_name(b))].push_back(atoms_ids_pair);
				inter_chain_interfaces[std::make_pair(contact_accepter->assign_group_name(b), contact_accepter->assign_group_name(a))].push_back(reversed_atoms_ids_pair);
			}
		}
	}

	if(inter_chain_interfaces.empty())
	{
		throw std::runtime_error("No interfaces found");
	}

	std::cout << "from pymol.cgo import *\n";
	std::cout << "from pymol import cmd\n\n";

	for(InterfacesMap::const_iterator it=inter_chain_interfaces.begin();it!=inter_chain_interfaces.end();++it)
	{
		const std::string obj_name=output_names_prefix+std::string("obj_")+it->first.first+"_"+it->first.second;
		const std::string cgo_name=output_names_prefix+std::string("iface_")+it->first.first+"_"+it->first.second;
		const auxiliaries::OpenGLPrinter opengl_printer(obj_name, cgo_name);
		for(std::size_t i=0;i<it->second.size();++i)
		{
			const std::pair<std::size_t, std::size_t> atoms_ids_pair=it->second[i];
			const protein::Atom& a=atoms[atoms_ids_pair.first];
			const protein::Atom& b=atoms[atoms_ids_pair.second];
			const CellFace& cell_face=faces_vector[faces_vector_map.find(atoms_ids_pair)->second];
			const apollo::SimplePoint normal=apollo::sub_of_points<apollo::SimplePoint>(b, a).unit();
			opengl_printer.print_tringle_fan(cell_face.mesh_vertices(), normal, face_colorizer->color(a, b));
		}
	}

	if(!selection_coloring_mode.empty())
	{
		bool color_pymol_selection_at_atomic_level=true;
		std::auto_ptr<const ContactColorizerInterface> selection_colorizer;
		if(selection_coloring_mode=="residue_type")
		{
			selection_colorizer.reset(new ContactColorizerByFirstResidueName<ResidueNameColorizerByResidueType>());
			color_pymol_selection_at_atomic_level=false;
		}
		else if(selection_coloring_mode=="residue_hydrophobicity")
		{
			selection_colorizer.reset(new ContactColorizerByFirstResidueName<ResidueNameColorizerByResidueHydrophobicity>());
			color_pymol_selection_at_atomic_level=false;
		}
		else if(selection_coloring_mode=="atom_type")
		{
			selection_colorizer.reset(new ContactColorizerByFirstAtomName());
		}
		else if(selection_coloring_mode=="residue_id")
		{
			selection_colorizer.reset(new ContactColorizerByFirstResidueID(atoms));
			color_pymol_selection_at_atomic_level=false;
		}
		else if(selection_coloring_mode=="atom_id")
		{
			selection_colorizer.reset(new ContactColorizerByFirstAtomID(atoms));
		}
		else
		{
			selection_colorizer.reset(new ContactColorizerByFirstResidueName< auxiliaries::NameColorizerForPymol<std::string> >());
			color_pymol_selection_at_atomic_level=false;
		}

		std::cout << "cmd.color('gray')\n\n";
		std::cout << "cmd.hide('nonbonded')\n\n";
		std::cout << "cmd.hide('lines')\n\n";
		std::cout << "cmd.show('ribbon')\n\n";

		selection_colorizer->list_colors();

		for(InterfacesMap::const_iterator it=inter_chain_interfaces.begin();it!=inter_chain_interfaces.end();++it)
		{
			const std::string selection_name=output_names_prefix+std::string("iface_sel_")+it->first.first+"_"+it->first.second;

			std::map< protein::ResidueID, std::vector< std::pair<std::size_t, std::size_t> > > selectable_residue_ids;
			for(std::size_t i=0;i<it->second.size();++i)
			{
				const std::pair<std::size_t, std::size_t>& atoms_ids_pair=it->second[i];
				const protein::Atom& a=atoms[atoms_ids_pair.first];
				selectable_residue_ids[protein::ResidueID::from_atom(a)].push_back(atoms_ids_pair);
			}

			std::cout << "cmd.select('" << selection_name << "', '";
			for(std::map< protein::ResidueID, std::vector< std::pair<std::size_t, std::size_t> > >::const_iterator jt=selectable_residue_ids.begin();jt!=selectable_residue_ids.end();++jt)
			{
				const protein::ResidueID& rid=jt->first;
				if(jt!=selectable_residue_ids.begin())
				{
					std::cout << " or ";
				}
				std::cout << "resi " << rid.residue_number << " and chain " << rid.chain_id;
			}
			std::cout << "')\n\n";

			for(std::map< protein::ResidueID, std::vector< std::pair<std::size_t, std::size_t> > >::const_iterator jt=selectable_residue_ids.begin();jt!=selectable_residue_ids.end();++jt)
			{
				const std::vector< std::pair<std::size_t, std::size_t> >& atoms_ids_pairs=jt->second;
				if(color_pymol_selection_at_atomic_level)
				{
					for(std::size_t i=0;i<atoms_ids_pairs.size();i++)
					{
						const std::pair<std::size_t, std::size_t>& atoms_ids_pair=atoms_ids_pairs[i];
						const protein::Atom& a=atoms[atoms_ids_pair.first];
						const protein::Atom& b=atoms[atoms_ids_pair.second];
						std::cout << "cmd.color('" << selection_colorizer->color_string(a, b) << "', 'resi " << a.residue_number << " and name " << (a.atom_name) << " and chain " << a.chain_id << "')\n";
					}
				}
				else if(!atoms_ids_pairs.empty())
				{
					const std::pair<std::size_t, std::size_t>& atoms_ids_pair=atoms_ids_pairs.front();
					const protein::Atom& a=atoms[atoms_ids_pair.first];
					const protein::Atom& b=atoms[atoms_ids_pair.second];
					std::cout << "cmd.color('" << selection_colorizer->color_string(a, b) << "', 'resi " << a.residue_number << " and chain " << a.chain_id << "')\n";
				}
			}
			std::cout << "\n";

			std::cout << "cmd.show('sticks', '" << selection_name << "')\n\n";
		}

		std::cout << "cmd.deselect()\n\n";
		std::cout << "cmd.center()\n\n";
		std::cout << "cmd.zoom()\n\n";

		std::cout << "cmd.set('ray_shadows', 'off')\n\n";
	}
}
