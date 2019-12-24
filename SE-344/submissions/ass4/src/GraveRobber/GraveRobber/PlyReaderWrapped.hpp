#pragma once
#define TINYPLY_IMPLEMENTATION
#include "PlyLoader/tinyply.h"
// #include "Triangle/Triangle.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

using namespace tinyply;

class manual_timer {
    std::chrono::high_resolution_clock::time_point t0;
    double                                         timestamp{ 0.f };

public:
    void start() {
        t0 = std::chrono::high_resolution_clock::now();
    }
    void stop() {
        timestamp = std::chrono::duration< float >( std::chrono::high_resolution_clock::now() - t0 ).count() * 1000;
    }
    const double& get() {
        return timestamp;
    }
};

struct float2 {
    float x, y;
};
struct float3 {
    float x, y, z;
};
struct double3 {
    double x, y, z;
};
struct uint3 {
    uint32_t x, y, z;
};
struct uint4 {
    uint32_t x, y, z, w;
};
struct triangle {
    float3 a, b, c;
};
struct geometry {
    std::vector< float3 > vertices;
    std::vector< float3 > normals;
    std::vector< float2 > texcoords;
    std::vector< uint3 >  triangles;
};

std::vector< triangle > read_ply_file( const std::string& filepath ) {

    std::vector< triangle > returns;
    try {
        std::ifstream ss( filepath, std::ios::binary );
        if ( ss.fail() )
            throw std::runtime_error( "failed to open " + filepath );

        PlyFile file;
        file.parse_header( ss );

        std::cout << "........................................................................\n";
        for ( auto c : file.get_comments() )
            std::cout << "Comment: " << c << std::endl;
        for ( auto e : file.get_elements() ) {
            std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
            for ( auto p : e.properties )
                std::cout << "\tproperty - " << p.name << " (" << tinyply::PropertyTable[ p.propertyType ].str << ")" << std::endl;
        }
        std::cout << "........................................................................\n";

        // Tinyply treats parsed data as untyped byte buffers. See below for examples.
        std::shared_ptr< PlyData > vertices, normals, faces, texcoords;

        // The header information can be used to programmatically extract properties on elements
        // known to exist in the header prior to reading the data. For brevity of this sample, properties
        // like vertex position are hard-coded:
        try {
            vertices = file.request_properties_from_element( "vertex", { "x", "y", "z" } );
        }
        catch ( const std::exception& e ) {
            std::cerr << "tinyply exception: " << e.what() << std::endl;
        }

        try {
            normals = file.request_properties_from_element( "vertex", { "nx", "ny", "nz" } );
        }
        catch ( const std::exception& e ) {
            std::cerr << "tinyply exception: " << e.what() << std::endl;
        }

        try {
            texcoords = file.request_properties_from_element( "vertex", { "u", "v" } );
        }
        catch ( const std::exception& e ) {
            std::cerr << "tinyply exception: " << e.what() << std::endl;
        }

        // Providing a list size hint (the last argument) is a 2x performance improvement. If you have
        // arbitrary ply files, it is best to leave this 0.
        try {
            faces = file.request_properties_from_element( "face", { "vertex_indices" }, 3 );
        }
        catch ( const std::exception& e ) {
            std::cerr << "tinyply exception: " << e.what() << std::endl;
        }

        manual_timer read_timer;

        read_timer.start();
        file.read( ss );
        read_timer.stop();

        std::cout << "Reading took " << read_timer.get() / 1000.f << " seconds." << std::endl;
        if ( vertices )
            std::cout << "\tRead " << vertices->count << " total vertices " << std::endl;
        if ( normals )
            std::cout << "\tRead " << normals->count << " total vertex normals " << std::endl;
        if ( texcoords )
            std::cout << "\tRead " << texcoords->count << " total vertex texcoords " << std::endl;
        if ( faces )
            std::cout << "\tRead " << faces->count << " total faces (triangles) " << std::endl;

        const size_t          numVerticesBytes = vertices->buffer.size_bytes();
        std::vector< float3 > verts( vertices->count );
        std::memcpy( verts.data(), vertices->buffer.get(), numVerticesBytes );

        const size_t         numFacesBytes = faces->buffer.size_bytes();
        std::vector< uint3 > trigs( faces->count );
        std::memcpy( trigs.data(), faces->buffer.get(), numFacesBytes );

        for ( size_t i = 0; i < trigs.size(); ++i ) {
            auto raw = trigs[ i ];
            auto t   = triangle();
            t.a      = verts[ raw.x ];
            t.b      = verts[ raw.y ];
            t.c      = verts[ raw.z ];
            returns.push_back( t );
        }
        return returns;
    }
    catch ( const std::exception& e ) {
        std::cerr << "Caught tinyply exception: " << e.what() << std::endl;
        returns.clear();
        return returns;
    }
}