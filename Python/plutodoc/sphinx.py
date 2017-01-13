import sys

def generate_docs(force=False):
    if not hasattr(sys, 'argv'):
        sys.argv = []
    import sphinx.application
    import sphinx
    app = sphinx.application.Sphinx('../Doc/source', '../Doc/source', '../Doc/build', '../Doc/build/doctrees/', 'html')
    app.build(force)