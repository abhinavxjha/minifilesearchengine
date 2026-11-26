/*
 * script.js — Mini Search Engine Client (Minimal Vanilla JS)
 *
 * All indexing, hashing, prefix matching, relevance scoring, and
 * merge sort ranking are strictly implemented in C++.
 * This script only sends fetch requests and renders JSON responses.
 */

document.addEventListener('DOMContentLoaded', () => {
    const searchInput = document.getElementById('search-input');
    const searchBtn = document.getElementById('search-btn');
    const clearBtn = document.getElementById('clear-btn');
    const autocompleteDropdown = document.getElementById('autocomplete-dropdown');
    const searchMeta = document.getElementById('search-meta');
    const resultsContainer = document.getElementById('results-container');
    const resultsCount = document.getElementById('results-count');
    const resultsQuery = document.getElementById('results-query');
    const searchLatency = document.getElementById('search-latency');

    let debounceTimer = null;
    let selectedIndex = -1;

    // ── Tab Switching ─────────────────────────────────────────
    document.querySelectorAll('.nav-tab').forEach(tab => {
        tab.addEventListener('click', () => {
            const targetId = tab.dataset.tab;
            document.querySelectorAll('.nav-tab').forEach(t => t.classList.remove('active'));
            document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
            tab.classList.add('active');
            document.getElementById(targetId)?.classList.add('active');
            if (targetId === 'documents-view') loadDocuments();
        });
    });

    // ── Search (C++ Inverted Index & MergeSort) ───────────────
    async function performSearch(query) {
        query = query.trim();
        if (!query) return;

        hideAutocomplete();
        resultsContainer.innerHTML = '<div class="no-results">Searching C++ Inverted Index...</div>';
        searchMeta.classList.remove('hidden');
        resultsQuery.textContent = `"${query}"`;
        resultsCount.textContent = '...';
        searchLatency.textContent = '...';

        try {
            const res = await fetch(`/api/search?q=${encodeURIComponent(query)}`);
            const data = await res.json();
            resultsCount.textContent = data.total;
            searchLatency.textContent = data.timeMs.toFixed(2);

            if (data.total === 0) {
                resultsContainer.innerHTML = `<div class="no-results">No documents matched <strong>"${escapeHtml(data.query)}"</strong>.</div>`;
                return;
            }

            resultsContainer.innerHTML = data.results.map((r, i) => `
                <article class="result-card">
                    <div class="result-header">
                        <div class="result-title"><span class="doc-badge">Doc #${r.docId}</span> ${escapeHtml(r.filename)}</div>
                        <div class="score-badge">Score: ${r.score.toFixed(1)}</div>
                    </div>
                    <div class="result-snippet">${r.snippet}</div>
                    <div class="result-footer">
                        <span style="color:var(--text-muted);">Rank #${i + 1} &bull; Matched:</span>
                        ${r.matchedWords.map(w => `<span class="term-chip">${escapeHtml(w)}</span>`).join(' ')}
                    </div>
                </article>`).join('');
        } catch {
            resultsContainer.innerHTML = '<div class="no-results" style="color:#ef4444">Error connecting to C++ engine.</div>';
        }
    }

    // ── Autocomplete (C++ Trie Prefix Tree) ───────────────────
    async function fetchAutocomplete(prefix) {
        if (!prefix) return hideAutocomplete();
        try {
            const res = await fetch(`/api/autocomplete?prefix=${encodeURIComponent(prefix)}`);
            const data = await res.json();
            if (!data.suggestions || data.suggestions.length === 0) return hideAutocomplete();

            selectedIndex = -1;
            autocompleteDropdown.innerHTML = data.suggestions.map((s, idx) => `
                <div class="autocomplete-item" data-index="${idx}" data-val="${escapeHtml(s)}">
                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="#68739f" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
                        <circle cx="11" cy="11" r="8"></circle><line x1="21" y1="21" x2="16.65" y2="16.65"></line>
                    </svg>
                    <span>${escapeHtml(s)}</span>
                </div>`
            ).join('');
            autocompleteDropdown.classList.remove('hidden');

            autocompleteDropdown.querySelectorAll('.autocomplete-item').forEach(item => {
                item.addEventListener('click', () => {
                    searchInput.value = item.dataset.val;
                    performSearch(item.dataset.val);
                });
            });
        } catch { hideAutocomplete(); }
    }

    function hideAutocomplete() {
        autocompleteDropdown.classList.add('hidden');
        autocompleteDropdown.innerHTML = '';
        selectedIndex = -1;
    }

    // ── Search Bar Input & Keyboard Events ────────────────────
    searchInput.addEventListener('input', (e) => {
        const val = e.target.value;
        clearBtn.classList.toggle('hidden', !val);
        clearTimeout(debounceTimer);
        const words = val.trim().split(/\s+/);
        const last = words[words.length - 1];
        debounceTimer = setTimeout(() => fetchAutocomplete(last), 150);
    });

    searchInput.addEventListener('keydown', (e) => {
        const items = autocompleteDropdown.querySelectorAll('.autocomplete-item');
        if (!autocompleteDropdown.classList.contains('hidden') && items.length > 0) {
            if (e.key === 'ArrowDown') {
                e.preventDefault();
                selectedIndex = (selectedIndex + 1) % items.length;
                updateSelection(items);
                return;
            } else if (e.key === 'ArrowUp') {
                e.preventDefault();
                selectedIndex = (selectedIndex - 1 + items.length) % items.length;
                updateSelection(items);
                return;
            } else if (e.key === 'Enter' && selectedIndex >= 0) {
                e.preventDefault();
                searchInput.value = items[selectedIndex].dataset.val;
                performSearch(searchInput.value);
                return;
            }
        }

        if (e.key === 'Enter') performSearch(searchInput.value);
        if (e.key === 'Escape') hideAutocomplete();
    });

    function updateSelection(items) {
        items.forEach((item, idx) => item.classList.toggle('selected', idx === selectedIndex));
        if (selectedIndex >= 0) searchInput.value = items[selectedIndex].dataset.val;
    }

    searchBtn.addEventListener('click', () => performSearch(searchInput.value));

    clearBtn.addEventListener('click', () => {
        searchInput.value = '';
        clearBtn.classList.add('hidden');
        hideAutocomplete();
        searchMeta.classList.add('hidden');
        resultsContainer.innerHTML = '';
        searchInput.focus();
    });

    document.querySelectorAll('.quick-chip').forEach(chip => {
        chip.addEventListener('click', () => {
            searchInput.value = chip.dataset.query;
            clearBtn.classList.remove('hidden');
            performSearch(chip.dataset.query);
        });
    });

    document.addEventListener('click', (e) => {
        if (!searchInput.contains(e.target) && !autocompleteDropdown.contains(e.target)) hideAutocomplete();
    });

    // ── Documents Loader ──────────────────────────────────────
    async function loadDocuments() {
        const container = document.getElementById('documents-list-container');
        if (!container) return;
        try {
            const res = await fetch('/api/documents');
            const data = await res.json();
            container.innerHTML = data.documents.map(d => `
                <div class="doc-card">
                    <div class="doc-card-header">
                        <span class="doc-card-title">${escapeHtml(d.filename)}</span>
                        <span class="doc-badge">ID: ${d.id}</span>
                    </div>
                    <div class="doc-card-stats">${d.wordCount} words &bull; Tokenized &amp; Indexed</div>
                    <div class="doc-card-preview">${escapeHtml(d.preview)}</div>
                </div>`).join('');
        } catch {
            container.innerHTML = '<div class="no-results" style="color:#ef4444">Error loading documents.</div>';
        }
    }

    function escapeHtml(str) {
        return (str || '').toString().replace(/[&<>"']/g, m => ({ '&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#039;' })[m]);
    }
});
