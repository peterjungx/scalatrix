#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <tuple>

// Node structure
struct Node {
    double x, y, pitch;
    bool operator<(const Node& other) const { return y < other.y; }
};
struct IntNode {
    int x,y;
};

// Affine Transform (2x2 matrix + translation, but we ignore translation here)
struct AffineTransform {
    double a, b, c, d;
    AffineTransform(double a_, double b_, double c_, double d_) : a(a_), b(b_), c(c_), d(d_) {}
    AffineTransform operator*(const double s) const {
        return {a * s, b * s, c * s, d * s};
    }
    AffineTransform inverse() const {
        double det = a * d - b * c;
        assert(std::abs(det) > 1e-7f);
        return {d / det, -b / det, -c / det, a / det};
    }
    Node apply(const Node& node) const {
        return {a * node.x + b * node.y, c * node.x + d * node.y, node.pitch};
    }
};


double isnull (double x) {
    return std::abs(x) < 1e-6;
}   

std::tuple<IntNode, IntNode> findClosestWithinStrip(AffineTransform& M,AffineTransform& M_inv) {
    // finds the two linearly independent nodes within the strip
    // that are closest to x=0
    // per the 3-gap theorem, all gaps are r.x, s.x or r.x+s.x
    // the return values have r.x < s.x, i.e. r is closer to x=0 than s

    Node v = {1,0,0};
    Node r = M_inv.apply(v);
    // print r0
    //printf("r_0 = (%f, %f) -> (%f,%f)\n", r.x, r.y, v.x, v.y);

    int r1, r2, s1, s2;
    Node z;
    bool has_first = false, has_second = false;
    if (isnull(r.x)){
        // edge case 1: r.x = 0
        s1 = 0;
        s2 = r.y>0?1:-1;
        has_first = true;
        z = M.apply({1, 0, 0.});
        if (std::abs(z.y) < 1){
            r1 = z.y>0?1:-1;
            r2 = 0;
            has_second = true;
        }else{
            r1 = s1;
            r2 = s2;
        }
    }
    else if (isnull(r.y)){
        // edge case 2: r.y = 0
        s1 = r.x>0?1:-1;
        s2 = 0;
        has_first = true;
        z = M.apply({0, 1, 0.});
        if (std::abs(z.y) < 1){
            r1 = 0;
            r2 = z.y>0?1:-1;
            has_second = true;
        }else{
            r1 = s1;
            r2 = s2;
        }
    }
    else {
        // general case
        // find two linearly independent nodes within the strip -1<y<1
        // for this, do continued fraction for the vector r = M^-1 * (1,0)

        double e;
        bool x_large = std::abs(r.x) > std::abs(r.y);
        if (x_large){
            e = std::abs(r.y/r.x);
        }else{
            e = std::abs(r.x/r.y);
        }

        int a = std::floor(e);
        std::vector<int> num = {1, a};
        std::vector<int> den = {0, 1};
        double f = e - a;

        // continued fraction up to 20 terms is more than enough for any sensible input
        while (num.size() < 20) {
            if (x_large){
                r1 = den.back();
                r2 = (r.x*r.y>0?1:-1) * num.back();
            }else{
                r1 = num.back();
                r2 = (r.x*r.y>0?1:-1) * den.back();
            }
            z = M.apply({(double)r1, (double)r2, 0.});
            if (z.x < 0){
                z.x = -z.x;
                z.y = -z.y;
                r1 = -r1;
                r2 = -r2;
            }
            //printf("%d/%d -> (%f,%f)\n", r2, r1, z.x, z.y);
            if (z.y < 1 && z.y > -1){
                if(!has_first){
                    has_first = true;
                    s1 = r1;
                    s2 = r2;
                }
                else{
                    has_second = true;
                    break;
                }
            }
            a = std::floor(1/f);
            f = 1/f - a;
            num.push_back(a*num[num.size()-1] + num[num.size()-2]);
            den.push_back(a*den[den.size()-1] + den[den.size()-2]);

        }

    }
    //std::cout << "\n";
    if (!has_first){
        return {{0,0}, {0,0}};
    }
    if (!has_second){
        return {{s1,s2}, {s1,s2}};
    }

    Node zr, zs;
    zr = M.apply({(double)r1, (double)r2, 0.});
    zs = M.apply({(double)s1, (double)s2, 0.});
    if (zr.x > zs.x){
        std::swap(r1, s1);
        std::swap(r2, s2);
        std::swap(zr, zs);
    }
    // zr.x < zs.x

    // recursively find nodes closest to x=0
    // the nodes we start with (call them r_0 and s_0) can be written in terms of the true minimal r,s
    // as r_0 = a*r+b*s and s_0 = c*r+d*s (for some integers a,b,c,d) 
    // as per the 3-gap theorem. They are guaranteed to be linearly independent 
    // because of the continued fraction algorithm above.
    // the following recursion must yield the true minimal r,s:
    // Start with vectors r_0 and s_0 with r_0.x < s_0.x.
    // If r_0 and s_0 are linearly independent, r_1=r_0 and s_1=s_0-r_0 are too, 
    // and s_1.x < s_0.x. s_1.x < r_1.x , swap r_1 and s_1.

    bool changed = true;
    int t1, t2, cnt;
    while (changed){
        changed = false;
        cnt = 0;
        if (zs.x>0){
            while(zs.x > 0 && zs.y > -1 && zs.y < 1){
                t1 = s1;
                t2 = s2;
                s1 -= r1;
                s2 -= r2;
                zs = M.apply({(double)s1,(double)s2,0.});
                changed = true;
                cnt++;
            }
            if (cnt==1) changed = false;
            s1 = t1;
            s2 = t2;
            //zr = M.apply({(double)r1, (double)r2, 0.});
            zs = M.apply({(double)s1,(double)s2,0.});             
            if (zr.x > zs.x){
                std::swap(r1, s1);
                std::swap(r2, s2);
                std::swap(zr, zs);
            }   
        }else{
            break;
        }
    } 
    assert(zr.x >= 0 && zr.x + zs.x > 0);
    assert(zr.x <= zs.x);
    return {IntNode{r1, r2}, IntNode{s1, s2}};
    
}


// Scale class with generatePath
class Scale {
public:
    std::vector<Node> nodes;
    double base_freq = 261.63f; // Middle C

    void generatePath(AffineTransform M, int N, int n_root, double y0 = 0.1) {
        assert (y0>=0 && y0<1);

        auto M_inv = M.inverse();
        auto [r,s] = findClosestWithinStrip(M, M_inv);
        Node zr = M.apply({(double)r.x, (double)r.y, 0.});
        Node zs = M.apply({(double)s.x, (double)s.y, 0.});



        //printf("r = (%d, %d) -> (%f, %f)\n", r.x, r.y, zr.x, zr.y);
        //printf("s = (%d, %d) -> (%f, %f)\n", s.x, s.y, zs.x, zs.y);

        std::vector<Node> right;
        std::vector<Node> left;

        int n_min = -n_root, n_max = N - n_root;


        // generate the nodes within the strip using the 3-gap theorem.
        // incrementing the last node by one of r, s or r+s will be within the strip

        Node root = M_inv.apply({0, y0, base_freq});
        Node last = root;
        Node zlast = M.apply(last);
        Node nn, zn;

        // forward pass
        //printf("n_max = %d | zr.y = %f | zs.y = %f | zr.y+zs.y = %f \n", n_max, zr.y, zs.y, zr.y+zs.y);
        for (int n = 1; n < n_max; ++n) {
            if(zlast.y + zr.y >= 1 || zlast.y + zr.y < 0){
                if (zlast.y + zs.y >= 1 || zlast.y + zs.y < 0){
                    //printf("y(r+s): %f + %f = %f\n", zlast.y,  zr.y + zs.y,  zlast.y + zr.y + zs.y);    
                    last = {last.x + r.x + s.x, last.y + r.y + s.y, 0.};

                }else{
                    //printf("y(s): %f + %f = %f\n", zlast.y,  zs.y,  zlast.y + zs.y);
                    last = {last.x + s.x, last.y + s.y, 0.};
                }
            }else{
                //printf("y(r): %f + %f = %f\n", zlast.y,  zr.y,  zlast.y + zr.y);
                last = {last.x + r.x, last.y + r.y, 0.};
            }
            zlast = M.apply(last);
            right.push_back({zlast.x, zlast.y, base_freq * std::exp2(zlast.x)});
        }
        //std::cout << "Length of right: " << right.size() << "\n";

        // backward pass
        last = root;
        zlast = M.apply(last);
        for (int n = -1; n >= n_min; --n) {
            if(zlast.y - zr.y >= 1 || zlast.y - zr.y < 0){
                if (zlast.y - zs.y >= 1 || zlast.y - zs.y < 0){
                    last = {last.x - r.x - s.x, last.y - r.y - s.y, 0.};
                }else{
                    last = {last.x - s.x, last.y - s.y, 0.};
                }
            }else{
                last = {last.x - r.x, last.y - r.y, 0.};
            }
            zlast = M.apply(last);
            left.push_back({zlast.x, zlast.y, base_freq * std::exp2(zlast.x)});
        }
        // reverse left
        std::reverse(left.begin(), left.end());
        // Combine (nodes = left + root + right)
        nodes.clear();
        nodes = left;
        nodes.push_back(M.apply(root));
        nodes.insert(nodes.end(), right.begin(), right.end());
        // print length of nodes
        //std::cout << "Length of nodes: " << nodes.size() << "\n";
        
    }

    void print(int first=58, int num = 5) const {
        std::cout << "Nodes (" << num << " starting from "<< first <<"):\n";
        for (int i = first; i < first+std::min(num, static_cast<int>(nodes.size())); ++i) {
            std::cout << i << ": x=" << nodes[i].x << ", y=" << nodes[i].y 
                      << ", pitch=" << nodes[i].pitch << " Hz\n";
        }
    }
};

// Test function
void runTest(const std::string& name, AffineTransform M, int N = 128, int n_root = 60, double y0 = 0.1) {
    std::cout << "\nTest: " << name << "\n";
    std::cout << "M = [[" << M.a << ", " << M.b << "], [" << M.c << ", " << M.d << "]]\n";

    Scale scale;
    scale.generatePath(M, N, n_root, y0);
    scale.print();

    // Basic checks
    assert(scale.nodes.size() == N);
    for (int i = 0; i < N; ++i) {
        assert(scale.nodes[i].y >= 0 && scale.nodes[i].y < 1);
    }
    for (int i = 1; i < N; ++i) {
        assert(scale.nodes[i].x >= scale.nodes[i-1].x);
    }

    std::cout << "Test passed.\n";
}

void runTests(){

    // Test cases
    // 1. Standard 12-TET approximation (equal steps)
    runTest("12-TET Approx", AffineTransform(1.0f, 1.0f/12, 0.0001f, 1.0f)*100);

    // 2. Identity lattice (degenerate case)
    runTest("Identity", AffineTransform(1.0f, 0.0f, 0.0f, 1.0f)*.4, 128, 60, .1);

    // 3. Sheared lattice (MOS-like)
    runTest("Shear (MOS-like)", AffineTransform(1.0f, 0.5f, 0.0f, 1.0f));

    // 4. Rotated lattice (non-trivial)
    double theta = 0.3f; // Small rotation
    runTest("Rotation", AffineTransform(std::cos(theta), -std::sin(theta), 
                                        std::sin(theta), std::cos(theta))*.4);

    // 5. Challenging case (near-singular)
    runTest("Near Singular", AffineTransform(1.0f, 0.999f, 0.001f, 1.0f));
}


void runCircularTest(){
    double theta = 0.; 
    double factor, y0, range;

    for (int j=0;j<10;j++){
        factor = 0.1*j + 0.1;

        for (int i = 0; i < 31; i++){
            theta = 0.1f * i;
            for (int k=0;k<5;k++){
                y0 = 0.15*k + .01;        
                auto M =  AffineTransform(std::cos(theta), -std::sin(theta), 
                                                std::sin(theta), std::cos(theta)) * factor;
                Scale scale;
                scale.generatePath(M, 128, 60, y0);
                range = scale.nodes[127].x - scale.nodes[0].x;

                printf("%d %d %d - %f\n", j, k, i, range);
            }
        }
    }
}

int main() {
    runTests();
    //runCircularTest();

    return 0;
}