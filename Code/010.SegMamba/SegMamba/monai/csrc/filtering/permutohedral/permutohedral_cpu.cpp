/*
Copyright (c) MONAI Consortium
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
Adapted from https://github.com/abadams/permutohedral
which has the following license...

MIT License

Copyright (c) 2020 Andrew Adams

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <math.h>
#include <string.h>

#include <torch/extension.h>

using namespace std;

/***************************************************************/
/* Hash table implementation for permutohedral lattice
 *
 * The lattice points are stored sparsely using a hash table.
 * The key for each point is its spatial location in the (d+1)-
 * dimensional space.
 */
/***************************************************************/
template <typename scalar_t>
class HashTablePermutohedral {
 public:
  /* Constructor
   *  kd_: the dimensionality of the position vectors on the hyperplane.
   *  vd_: the dimensionality of the value vectors
   */
  HashTablePermutohedral(int kd_, int vd_) : kd(kd_), vd(vd_) {
    capacity = 1 << 15;
    filled = 0;
    entries = new Entry[capacity];
    keys = new short[kd * capacity / 2];
    values = new scalar_t[vd * capacity / 2];
    memset(values, 0, sizeof(scalar_t) * vd * capacity / 2);
  }

  // Returns the number of vectors stored.
  int size() {
    return filled;
  }

  // Returns a pointer to the keys array.
  short* getKeys() {
    return keys;
  }

  // Returns a pointer to the values array.
  scalar_t* getValues() {
    return values;
  }

  /* Returns the index into the hash table for a given key.
   *     key: a pointer to the position vector.
   *       h: hash of the position vector.
   *  create: a flag specifying whether an entry should be created,
   *          should an entry with the given key not found.
   */
  int lookupOffset(short* key, size_t h, bool create = true) {
    // Double hash table size if necessary
    if (filled >= (capacity / 2) - 1) {
      grow();
    }

    // Find the entry with the given key
    while (1) {
      Entry e = entries[h];
      // check if the cell is empty
      if (e.keyIdx == -1) {
        if (!create)
          return -1; // Return not found.
        // need to create an entry. Store the given key.
        for (int i = 0; i < kd; i++)
          keys[filled * kd + i] = key[i];
        e.keyIdx = filled * kd;
        e.valueIdx = filled * vd;
        entries[h] = e;
        filled++;
        return e.valueIdx;
      }

      // check if the cell has a matching key
      bool match = true;
      for (int i = 0; i < kd && match; i++)
        match = keys[e.keyIdx + i] == key[i];
      if (match)
        return e.valueIdx;

      // increment the bucket with wraparound
      h++;
      if (h == capacity)
        h = 0;
    }
  }

  /* Looks up the value vector associated with a given key vector.
   *        k : pointer to the key vector to be looked up.
   *   create : true if a non-existing key should be created.
   */
  scalar_t* lookup(short* k, bool create = true) {
    size_t h = hash(k) % capacity;
    int offset = lookupOffset(k, h, create);
    if (offset < 0)
      return NULL;
    else
      return values + offset;
  };

  /* Hash function used in this implementation. A simple base conversion. */
  size_t hash(const short* key) {
    size_t k = 0;
    for (int i = 0; i < kd; i++) {
      k += key[i];
      k *= 2531011;
    }
    return k;
  }

 private:
  /* Grows the size of the hash table */
  void grow() {
    size_t oldCapacity = capacity;
    capacity *= 2;

    // Migrate the value vectors.
    scalar_t* newValues = new scalar_t[vd * capacity / 2];
    memset(newValues, 0, sizeof(scalar_t) * vd * capacity / 2);
    memcpy(newValues, values, sizeof(scalar_t) * vd * filled);
    delete[] values;
    values = newValues;

    // Migrate the key vectors.
    short* newKeys = new short[kd * capacity / 2];
    memcpy(newKeys, keys, sizeof(short) * kd * filled);
    delete[] keys;
    keys = newKeys;

    Entry* newEntries = new Entry[capacity];

    // Migrate the table of indices.
    for (size_t i = 0; i < oldCapacity; i++) {
      if (entries[i].keyIdx == -1)
        continue;
      size_t h = hash(keys + entries[i].keyIdx) % capacity;
      while (newEntries[h].keyIdx != -1) {
        h++;
        if (h == capacity)
          h = 0;
      }
      newEntries[h] = entries[i];
    }
    delete[] entries;
    entries = newEntries;
  }

  // Private struct for the hash table entries.
  struct Entry {
    Entry() : keyIdx(-1), valueIdx(-1) {}
    int keyIdx;
    int valueIdx;
  };

  short* keys;
  scalar_t* values;
  Entry* entries;
  size_t capacity, filled;
  int kd, vd;
};

/***************************************************************/
/* The algorithm class that performs the filter
 *
 * PermutohedralLattice::filter(...) does all the work.
 *
 */
/***************************************************************/
template <typename scalar_t>
class PermutohedralLattice {
 public:
  /* Filters given image against a reference image.
   *   im : image to be bilateral-filtered.
   *  ref : reference image whose edges are to be respected.
   */
  static void filter(scalar_t* data, scalar_t* features, int dataChannels, int featureChannels, int elementCount) {
    // Create lattice
    PermutohedralLattice lattice(featureChannels, dataChannels + 1, elementCount);

    // Splat into the lattice
    scalar_t* col = new scalar_t[dataChannels + 1];
    col[dataChannels] = 1; // homogeneous coordinate

    for (int i = 0, e = 0; e < elementCount; e++) {
      for (int c = 0; c < dataChannels; c++, i++) {
        col[c] = data[i];
      }

      scalar_t* featureVec = features + e * featureChannels;
      lattice.splat(featureVec, col);
    }

    // Blur the lattice
    lattice.blur();

    // Slice from the lattice
    lattice.beginSlice();

    for (int i = 0, e = 0; e < elementCount; e++) {
      lattice.slice(col);

      scalar_t scale = 1.0f / col[dataChannels];
      for (int c = 0; c < dataChannels; c++, i++) {
        data[i] = col[c] * scale;
      }
    }
  }

  /* Constructor
   *     d_ : dimensionality of key vectors
   *    vd_ : dimensionality of value vectors
   * nData_ : number of points in the input
   */
  PermutohedralLattice(int d_, int vd_, int nData_) : d(d_), vd(vd_), nData(nData_), hashTable(d_, vd_) {
    // Allocate storage for various arrays
    elevated = new scalar_t[d + 1];
    scaleFactor = new scalar_t[d];

    greedy = new short[d + 1];
    rank = new char[d + 1];
    barycentric = new scalar_t[d + 2];
    replay = new ReplayEntry[nData * (d + 1)];
    nReplay = 0;
    canonical = new short[(d + 1) * (d + 1)];
    key = new short[d + 1];

    // compute the coordinates of the canonical simplex, in which
    // the difference between a contained point and the zero
    // remainder vertex is always in ascending order. (See pg.4 of paper.)
    for (int i = 0; i <= d; i++) {
      for (int j = 0; j <= d - i; j++)
        canonical[i * (d + 1) + j] = i;
      for (int j = d - i + 1; j <= d; j++)
        canonical[i * (d + 1) + j] = i - (d + 1);
    }

    // Compute parts of the rotation matrix E. (See pg.4-5 of paper.)
    for (int i = 0; i < d; i++) {
      // the diagonal entries for normalization
      scaleFactor[i] = 1.0f / (sqrtf((scalar_t)(i + 1) * (i + 2)));

      /* We presume that the user would like to do a Gaussian blur of standard deviation
       * 1 in each dimension (or a total variance of d, summed over dimensions.)
       * Because the total variance of the blur performed by this algorithm is not d,
       * we must scale the space to offset this.
       *
       * The total variance of the algorithm is (See pg.6 and 10 of paper):
       *  [variance of splatting] + [variance of blurring] + [variance of splatting]
       *   = d(d+1)(d+1)/12 + d(d+1)(d+1)/2 + d(d+1)(d+1)/12
       *   = 2d(d+1)(d+1)/3.
       *
       * So we need to scale the space by (d+1)sqrt(2/3).
       */
      scaleFactor[i] *= (d + 1) * sqrtf(2.0 / 3);
    }
  }

  /* Performs splatting with given position and value vectors */
  void splat(scalar_t* position, scalar_t* value) {
    // first rotate position into the (d+1)-dimensional hyperplane
    elevated[d] = -d * position[d - 1] * scaleFactor[d - 1];
    for (int i = d - 1; i > 0; i--)
      elevated[i] =
          (elevated[i + 1] - i * position[i - 1] * scaleFactor[i - 1] + (i + 2) * position[i] * scaleFactor[i]);
    elevated[0] = elevated[1] + 2 * position[0] * scaleFactor[0];

    // prepare to find the closest lattice points
    scalar_t scale = 1.0f / (d + 1);
    char* myrank = rank;
    short* mygreedy = greedy;

    // greedily search for the closest zero-colored lattice point
    int sum = 0;
    for (int i = 0; i <= d; i++) {
      scalar_t v = elevated[i] * scale;
      scalar_t up = ceilf(v) * (d + 1);
      scalar_t down = floorf(v) * (d + 1);

      if (up - elevated[i] < elevated[i] - down)
        mygreedy[i] = (short)up;
      else
        mygreedy[i] = (short)down;

      sum += mygreedy[i];
    }
    sum /= d + 1;

    // rank differential to find the permutation between this simplex and the canonical one.
    // (See pg. 3-4 in paper.)
    memset(myrank, 0, sizeof(char) * (d + 1));
    for (int i = 0; i < d; i++)
      for (int j = i + 1; j <= d; j++)
        if (elevated[i] - mygreedy[i] < elevated[j] - mygreedy[j])
          myrank[i]++;
        else
          myrank[j]++;

    if (sum > 0) {
      // sum too large - the point is off the hyperplane.
      // need to bring down the ones with the smallest differential
      for (int i = 0; i <= d; i++) {
        if (myrank[i] >= d + 1 - sum) {
          mygreedy[i] -= d + 1;
          myrank[i] += sum - (d + 1);
        } else
          myrank[i] += sum;
      }
    } else if (sum < 0) {
      // sum too small - the point is off the hyperplane
      // need to bring up the ones with largest differential
      for (int i = 0; i <= d; i++) {
        if (myrank[i] < -sum) {
          mygreedy[i] += d + 1;
          myrank[i] += (d + 1) + sum;
        } else
          myrank[i] += sum;
      }
    }

    // Compute barycentric coordinates (See pg.10 of paper.)
    memset(barycentric, 0, sizeof(scalar_t) * (d + 2));
    for (int i = 0; i <= d; i++) {
      barycentric[d - myrank[i]] += (elevated[i] - mygreedy[i]) * scale;
      barycentric[d + 1 - myrank[i]] -= (elevated[i] - mygreedy[i]) * scale;
    }
    barycentric[0] += 1.0f + barycentric[d + 1];

    // Splat the value into each vertex of the simplex, with barycentric weights.
    for (int remainder = 0; remainder <= d; remainder++) {
      // Compute the location of the lattice point explicitly (all but the last coordinate - it's redundant because they
      // sum to zero)
      for (int i = 0; i < d; i++)
        key[i] = mygreedy[i] + canonical[remainder * (d + 1) + myrank[i]];

      // Retrieve pointer to the value at this vertex.
      scalar_t* val = hashTable.lookup(key, true);

      // Accumulate values with barycentric weight.
      for (int i = 0; i < vd; i++)
        val[i] += barycentric[remainder] * value[i];

      // Record this interaction to use later when slicing
      replay[nReplay].offset = val - hashTable.getValues();
      replay[nReplay].weight = barycentric[remainder];
      nReplay++;
    }
  }

  // Prepare for slicing
  void beginSlice() {
    nReplay = 0;
  }

  /* Performs slicing out of position vectors. Note that the barycentric weights and the simplex
   * containing each position vector were calculated and stored in the splatting step.
   * We may reuse this to accelerate the algorithm. (See pg. 6 in paper.)
   */
  void slice(scalar_t* col) {
    scalar_t* base = hashTable.getValues();
    for (int j = 0; j < vd; j++)
      col[j] = 0;
    for (int i = 0; i <= d; i++) {
      ReplayEntry r = replay[nReplay++];
      for (int j = 0; j < vd; j++) {
        col[j] += r.weight * base[r.offset + j];
      }
    }
  }

  /* Performs a Gaussian blur along each projected axis in the hyperplane. */
  void blur() {
    // Prepare arrays
    short* neighbor1 = new short[d + 1];
    short* neighbor2 = new short[d + 1];
    scalar_t* newValue = new scalar_t[vd * hashTable.size()];
    scalar_t* oldValue = hashTable.getValues();
    scalar_t* hashTableBase = oldValue;

    scalar_t* zero = new scalar_t[vd];
    for (int k = 0; k < vd; k++)
      zero[k] = 0;

    // For each of d+1 axes,
    for (int j = 0; j <= d; j++) {
      // For each vertex in the lattice,
      for (int i = 0; i < hashTable.size(); i++) { // blur point i in dimension j
        short* key = hashTable.getKeys() + i * (d); // keys to current vertex
        for (int k = 0; k < d; k++) {
          neighbor1[k] = key[k] + 1;
          neighbor2[k] = key[k] - 1;
        }
        neighbor1[j] = key[j] - d;
        neighbor2[j] = key[j] + d; // keys to the neighbors along the given axis.

        scalar_t* oldVal = oldValue + i * vd;
        scalar_t* newVal = newValue + i * vd;

        scalar_t *vm1, *vp1;

        vm1 = hashTable.lookup(neighbor1, false); // look up first neighbor
        if (vm1)
          vm1 = vm1 - hashTableBase + oldValue;
        else
          vm1 = zero;

        vp1 = hashTable.lookup(neighbor2, false); // look up second neighbor
        if (vp1)
          vp1 = vp1 - hashTableBase + oldValue;
        else
          vp1 = zero;

        // Mix values of the three vertices
        for (int k = 0; k < vd; k++)
          newVal[k] = (0.25f * vm1[k] + 0.5f * oldVal[k] + 0.25f * vp1[k]);
      }
      scalar_t* tmp = newValue;
      newValue = oldValue;
      oldValue = tmp;
      // the freshest data is now in oldValue, and newValue is ready to be written over
    }

    // depending where we ended up, we may have to copy data
    if (oldValue != hashTableBase) {
      memcpy(hashTableBase, oldValue, hashTable.size() * vd * sizeof(scalar_t));
      delete[] oldValue;
    } else {
      delete[] newValue;
    }

    delete[] zero;
    delete[] neighbor1;
    delete[] neighbor2;
  }

 private:
  int d, vd, nData;
  scalar_t *elevated, *scaleFactor, *barycentric;
  short* canonical;
  short* key;

  // slicing is done by replaying splatting (ie storing the sparse matrix)
  struct ReplayEntry {
    int offset;
    scalar_t weight;
  } * replay;
  int nReplay, nReplaySub;

 public:
  char* rank;
  short* greedy;
  HashTablePermutohedral<scalar_t> hashTable;
};

template <typename scalar_t>
void PermutohedralCPU(scalar_t* data, scalar_t* features, int dataChannels, int featureChannels, int elementCount) {
  PermutohedralLattice<scalar_t>::filter(data, features, dataChannels, featureChannels, elementCount);
}

template void PermutohedralCPU(float* data, float* features, int dataChannels, int featureChannels, int elementCount);
template void PermutohedralCPU(double* data, double* features, int dataChannels, int featureChannels, int elementCount);
