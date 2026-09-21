using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Leella.Prelude.Core;

namespace Leella.Prelude.Editor;

public sealed class MapEditorView : Control
{
    private readonly List<PlatformSegment> _platforms = [];
    private Point? _dragStart;
    private Point? _dragCurrent;

    public IReadOnlyList<PlatformSegment> Platforms => _platforms;

    public MapEditorView()
    {
        Focusable = true;
        ClipToBounds = true;
    }

    public void ClearMap()
    {
        _platforms.Clear();
        _dragStart = null;
        _dragCurrent = null;
        InvalidateVisual();
    }

    public void LoadDemo()
    {
        _platforms.Clear();
        _platforms.AddRange(
        [
            new PlatformSegment { X1 = 90, Y1 = 520, X2 = 520, Y2 = 520 },
            new PlatformSegment { X1 = 620, Y1 = 430, X2 = 940, Y2 = 430 },
            new PlatformSegment { X1 = 1030, Y1 = 540, X2 = 1370, Y2 = 540 },
            new PlatformSegment { X1 = 370, Y1 = 320, X2 = 690, Y2 = 320 }
        ]);
        InvalidateVisual();
    }

    protected override void OnPointerPressed(PointerPressedEventArgs e)
    {
        base.OnPointerPressed(e);
        Focus();

        if (!e.GetCurrentPoint(this).Properties.IsLeftButtonPressed)
            return;

        var point = Snap(e.GetPosition(this));
        _dragStart = point;
        _dragCurrent = point;
        e.Pointer.Capture(this);
        InvalidateVisual();
    }

    protected override void OnPointerMoved(PointerEventArgs e)
    {
        base.OnPointerMoved(e);
        if (_dragStart is null)
            return;

        _dragCurrent = Snap(e.GetPosition(this));
        InvalidateVisual();
    }

    protected override void OnPointerReleased(PointerReleasedEventArgs e)
    {
        base.OnPointerReleased(e);

        if (_dragStart is { } start && _dragCurrent is { } end)
        {
            if (Math.Abs(end.X - start.X) >= 16)
            {
                _platforms.Add(new PlatformSegment
                {
                    X1 = Math.Min(start.X, end.X),
                    Y1 = start.Y,
                    X2 = Math.Max(start.X, end.X),
                    Y2 = start.Y
                });
            }
        }

        _dragStart = null;
        _dragCurrent = null;
        e.Pointer.Capture(null);
        InvalidateVisual();
    }

    public override void Render(DrawingContext context)
    {
        base.Render(context);

        context.FillRectangle(new SolidColorBrush(Color.Parse("#151820")), Bounds);

        var gridPen = new Pen(new SolidColorBrush(Color.Parse("#262B36")), 1);
        const int grid = 32;

        for (var x = 0; x < Bounds.Width; x += grid)
            context.DrawLine(gridPen, new Point(x, 0), new Point(x, Bounds.Height));

        for (var y = 0; y < Bounds.Height; y += grid)
            context.DrawLine(gridPen, new Point(0, y), new Point(Bounds.Width, y));

        var platformPen = new Pen(new SolidColorBrush(Color.Parse("#75C7F0")), 5);

        foreach (var platform in _platforms)
            context.DrawLine(platformPen, new Point(platform.X1, platform.Y1), new Point(platform.X2, platform.Y2));

        if (_dragStart is { } start && _dragCurrent is { } end)
        {
            var previewPen = new Pen(new SolidColorBrush(Color.Parse("#F1C75B")), 4);
            context.DrawLine(previewPen, start, new Point(end.X, start.Y));
        }
    }

    private static Point Snap(Point point)
    {
        const double grid = 16;
        return new Point(
            Math.Round(point.X / grid) * grid,
            Math.Round(point.Y / grid) * grid);
    }
}
