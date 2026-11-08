<h1>parallel-radix-sort-baseline</h1>
<p>Parallel RadixSort (Variante 1) vs. std::qsort baseline using oneAPI TBB</p>

<h2>Project description</h2>
<p>This project implements the first iteration of a parallel, non-comparison-based sorting algorithm on CPU:</p>
<ol>
  <li>Generates a dataset of <code>N = 10,000,000</code> pseudorandom 32-bit unsigned integers.</li>
  <li>
    <dl>
      <dt>Runs sequential baseline:</dt>
      <dd>Evaluates standard library <code>std::qsort</code> as reference execution time.</dd>
    </dl>
  </li>
  <li>
    <dl>
      <dt>Executes Variante 1 (Naive Parallel RadixSort):</dt>
      <dd>Fixed 8-bit radix width (256 buckets, 4 total passes for 32-bit keys).</dd>
      <dd>Decomposes data into equal blocks across <code>p</code> worker threads via <code>tbb::parallel_for</code>.</dd>
      <dd>Uses dynamic 2D standard vectors (<code>std::vector&lt;std::vector&lt;size_t&gt;&gt;</code>) for local histograms and global offsets.</dd>
    </dl>
  </li>
  <li>Verifies sorting correctness using <code>std::is_sorted</code> and prints Speedup and Efficiency.</li>
</ol>

<h2>Project files</h2>
<table>
  <tr>
    <th colspan="2">parallel-radix-sort-baseline</th>
  </tr>
  <tr>
    <td>CMakeLists.txt</td>
    <td>Build setup linking <code>TBB::tbb</code></td>
  </tr>
  <tr>
    <td>main.cpp</td>
    <td>Implementation of baseline <code>std::qsort</code> and <code>radix_sort_parallel_v1</code></td>
  </tr>
  <tr>
    <td>README.md</td>
    <td>Project documentation and benchmarking methodology</td>
  </tr>
</table>

<h2>Installation and launch</h2>
<ol>
  <b>
    <li>
      <dl>
        <dt>Clone the repository</dt>
        <dd>
          <code>git clone https://github.com/Kyoryuu-lab/parallel-radix-sort-baseline.git</code>
        </dd>
        <dd>
          <code>cd parallel-radix-sort-baseline</code>
        </dd>
      </dl>
    </li>
  </b>
  <b>
    <li>
      <dl>
        <dt>Compile the project</dt>
        <dd>
          <code>cmake -B build -DCMAKE_BUILD_TYPE=Release</code>
        </dd>
        <dd>
          <code>cmake --build build --config Release</code>
        </dd>
      </dl>
    </li>
  </b>
  <b>
    <li>
      <dl>
        <dt>Run the benchmark</dt>
        <dd>
          <code>./build/main</code>
        </dd>
      </dl>
    </li>
  </b>
</ol>

<h2>Example of console output</h2>
<samp>
  <p>
    ==========================================<br />
    PVA Aufgabe 2: RadixSort vs std::qsort<br />
    Array size: 10000000 elements<br />
    Hardware threads (p): 15<br />
    ==========================================<br />
    std::qsort:             1150.45 ms<br />
    TBB RadixSort (V1):     168.20 ms<br />
    Verification V1:        SUCCESS<br />
    Speedup (S = Tseq/Tpar): 6.84x<br />
    Efficiency (E = S/p):    45.6 %<br />
    ==========================================
  </p>
</samp>
