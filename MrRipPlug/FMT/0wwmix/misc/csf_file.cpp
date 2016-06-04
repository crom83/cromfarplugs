// csf_file.cpp: implementation of the Ccsf_file class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include "csf_file.h"
#include "string_conversion.h"

int read_int(const byte*& r)
{
	int v = *reinterpret_cast<const __int32*>(r);
	r += 4;
	return v;
}

void write_int(byte*& w, int v)
{
	*reinterpret_cast<__int32*>(w) = v;
	w += 4;
}

string read_string(const byte*& r)
{
	int cb_s = read_int(r);
	string s = to_lower(string(reinterpret_cast<const char*>(r), cb_s));
	r += cb_s;
	return s;
}

wstring read_wstring(const byte*& r)
{
	int cb_s = read_int(r);
	wstring s = wstring(reinterpret_cast<const wchar_t*>(r), cb_s);
	r += cb_s << 1;
	return s;
}

void write_string(byte*& w, string v)
{
	int l = v.length();
	write_int(w, l);
	memcpy(w, v.c_str(), l);
	w += l;
}

void write_wstring(byte*& w, wstring v)
{
	int l = v.length();
	write_int(w, l);
	memcpy(w, v.c_str(), l << 1);
	w += l << 1;
}

int Ccsf_file::post_open()
{
	const byte* r = get_data() + sizeof(t_csf_header);
	for (int i = 0; i < get_header()->count1; i++)
	{
		read_int(r);
		read_int(r);
		string name = read_string(r);
		bool has_extra_value = read_int(r) == csf_string_w_id;
		wstring value = read_wstring(r);
		string extra_value;
		if (has_extra_value)
			extra_value = read_string(r);
		set_value(name, value, extra_value);
	}
	return 0;
}

string Ccsf_file::convert2string(const wstring& s)
{
	string r;
	for (int i = 0; i < s.length(); i++)
	{
		r += ~s[i];
	}
	return r;
}

wstring Ccsf_file::convert2wstring(const string& s)
{
	wstring r;
	for (int i = 0; i < s.length(); i++)
	{
		r += 0xff00 | ~s[i];
	}
	return r;
}

void Ccsf_file::erase_value(const string& name)
{
	m_map.erase(name);
}

string Ccsf_file::get_converted_value(const string& name) const
{
	return convert2string(get_value(name));
}

void Ccsf_file::set_value(const string& name, const wstring& value, const string& extra_value)
{
	t_map_entry e;
	e.value = value;
	e.extra_value = extra_value;
	m_map[name] = e;
}

int Ccsf_file::get_write_size() const
{
	int r = sizeof(t_csf_header);
	for (t_map::const_iterator i = m_map.begin(); i != m_map.end(); i++)
	{
		r += 20 + i->first.length() + (i->second.value.length() << 1);
		if (!i->second.extra_value.empty())
			r += 4 + i->second.extra_value.length();
	}
	return r;
}

void Ccsf_file::write(byte* d) const
{
	byte* w = d;
	t_csf_header& header = *reinterpret_cast<t_csf_header*>(w);
	w += sizeof(t_csf_header);
	header.id = csf_file_id;
	header.flags1 = 3;
	header.count1 = get_c_strings();
	header.count2 = get_c_strings();
	header.zero = 0;
	header.flags2 = 0;
	for (t_map::const_iterator i = m_map.begin(); i != m_map.end(); i++)
	{
		write_int(w, csf_label_id);
		write_int(w, 1);
		write_string(w, i->first);
		write_int(w, i->second.extra_value.empty() ? csf_string_id : csf_string_w_id);
		write_wstring(w, i->second.value);
		if (!i->second.extra_value.empty())
			write_string(w, i->second.extra_value);
	}
	assert(w - d == get_write_size());
}